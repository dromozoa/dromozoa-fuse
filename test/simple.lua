-- Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-fuse.
--
-- dromozoa-fuse is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-fuse is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-fuse.  If not, see <http://www.gnu.org/licenses/>.

local unix = require "dromozoa.unix"
local fuse = require "dromozoa.fuse"

local uid = unix.getuid();
local gid = unix.getgid();

local current_time

local function update_current_time()
  current_time = assert(unix.clock_gettime(unix.CLOCK_REALTIME))
end

update_current_time()

local function mode_dir(mode)
  return unix.bor(unix.S_IFDIR, tonumber(mode, 8))
end

local function mode_file(mode)
  return unix.bor(unix.S_IFREG, tonumber(mode, 8))
end

local function is_dir(node)
  return unix.band(node.attr.st_mode, unix.S_IFDIR) ~= 0
end

local function is_empty_dir(node)
  local m = 0
  local n = 0
  for name in pairs(node.nodes) do
    m = m + 1
    if name == "." or name == ".." then
      n = n + 1
    end
    if m > 2 then
      return false
    end
  end
  return m == 2 and n == 2
end

local function update_atime(node)
  node.attr.st_atime = current_time
  return node
end

local function update_mtime(node)
  node.attr.st_mtime = current_time
  return node
end

local function update_ctime(node)
  node.attr.st_ctime = current_time
  return node
end

local function increment_link(node)
  local attr = node.attr
  attr.st_nlink = attr.st_nlink + 1
  return update_ctime(node)
end

local function decrement_link(node)
  local attr = node.attr
  attr.st_nlink = attr.st_nlink - 1
  return update_ctime(node)
end

local vfs = {}

local root = {
  attr = {
    st_mode = mode_dir "0755";
    st_nlink = 1;
    st_uid = uid;
    st_gid = gid;
    st_atime = current_time;
    st_mtime = current_time;
    st_ctime = current_time;
    st_blocks = 0;
  };
  xattr = {};
  nodes = {
    [".."] = true;
  };
}

local function split(path)
  if path == "/" then
    error(-unix.EEXIST, 0)
  end
  local parent_path, name = path:match "(.*)/([^/]+)$"
  if #parent_path == 0 then
    return "/", name
  else
    return parent_path, name
  end
end

local function get(path)
  local this = root
  for name in path:gmatch "/([^/]+)" do
    this = this.nodes[name]
    if not this then
      error(-unix.ENOENT, 0)
    end
  end
  return this
end

local function set(path, node)
  local names = {}
  local n = 0
  for name in path:gmatch "/([^/]+)" do
    n = n + 1
    names[n] = name
  end
  if n == 0 then
    error(-unix.EEXIST, 0)
  end
  local this = root
  for i = 1, n - 1 do
    this = this.nodes[names[i]]
    if not this then
      error(-unix.ENOENT, 0)
    end
  end
  local name = names[n]
  local nodes = this.nodes
  if node then
    if nodes[name] then
      error(-unix.EEXIST, 0)
    end
  else
    if not nodes[name] then
      error(-unix.ENOENT, 0)
    end
  end
  nodes[name] = node
end

local function link(oldpath, newpath)
  set(newpath, increment_link(get(oldpath)))
end

local function unlink(path)
  local node = get(path)
  local parent_path = split(path)
  local parent_node = get(parent_path)
  set(path)
  decrement_link(node)
  update_mtime(parent_node)
  return node
end

local function rmdir(path)
  local node = get(path)
  local parent_path = split(path)
  local parent_node = get(parent_path)
  set(path .. "/..")
  set(path .. "/.")
  set(path)
  decrement_link(node)
  decrement_link(node)
  decrement_link(parent_node)
  update_mtime(parent_node)
  return node
end

local function mkdir(path, node)
  local parent_path = split(path)
  local parent_node = get(parent_path)
  set(path, node)
  link(parent_path, path .. "/..")
  link(path, path .. "/.")
  update_mtime(parent_node)
end

link("/", "/.")

local operations = {}

function operations:getattr(path)
  return get(path).attr
end

function operations:unlink(path)
  update_current_time()
  unlink(path)
end

function operations:rmdir(path)
  local node = get(path)

  if not is_dir(node) then
    error(-unix.ENOTDIR, 0)
  end
  if not is_empty_dir(node) then
    error(-unix.ENOTEMPTY, 0)
  end

  update_current_time()
  rmdir(path)
end

function operations:mkdir(path)
  update_current_time()
  mkdir(path, {
    attr = {
      st_mode = mode_dir "0755";
      st_nlink = 1;
      st_uid = uid;
      st_gid = gid;
      st_atime = current_time;
      st_mtime = current_time;
      st_ctime = current_time;
      st_blocks = 0;
    };
    xattr = {};
    nodes = {};
  })
end

function operations:rename(oldpath, newpath)
  local old_node = get(oldpath)
  local parent_path, name = split(newpath)
  local parent_node = get(parent_path)
  local node = parent_node.nodes[name]

  update_current_time()
  if is_dir(old_node) then
    if node then
      if not is_dir(node) then
        error(-unix.ENOTDIR, 0)
      end
      if not is_empty_dir(node) then
        error(-unix.ENOTEMPTY, 0)
      end
      rmdir(newpath)
    end
    rmdir(oldpath)
    increment_link(old_node)
    mkdir(newpath, old_node)
  else
    if node then
      if is_dir(node) then
        error(-unix.EISDIR, 0)
      end
      unlink(newpath)
    end
    unlink(oldpath)
    increment_link(old_node)
    set(newpath, old_node)
  end
end

function operations:chmod(path, mode)
  local node = get(path)
  local attr = node.attr
  update_current_time()
  attr.st_mode = unix.bor(unix.band(attr.st_mode, unix.S_IFMT), mode)
  update_ctime(node)
end

function operations:read(path, size, offset)
  local node = get(path)
  update_current_time()
  local result = node.content:get(offset, size)
  update_atime(node)
  return result
end

function operations:write(path, buffer, offset)
  local node = get(path)
  local content = node.content
  update_current_time()
  content:put(offset, buffer)
  node.attr.st_size = #content
  update_mtime(node)
end

function operations:statfs(path)
  return vfs
end

function operations:setxattr(path, name, value, position)
  local node = get(path)
  update_current_time()
  node.xattr[name] = value
  update_ctime(node)
end

function operations:getxattr(path, name, size, position)
  local node = get(path)
  local value = node.xattr[name]
  if not value then
    error(-unix.ENODATA, 0)
  end
  if size == 0 then
    return #value
  else
    return value
  end
end

function operations:listxattr(path, size)
  local node = get(path)
  local names = {}
  for name in pairs(node.xattr) do
    names[#names + 1] = name .. "\0"
  end
  local result = table.concat(names)
  if size == 0 then
    return #result
  else
    return result
  end
end

function operations:removexattr(path, name)
  local node = get(path)
  local xattr = node.xattr
  if not xattr[name] then
    error(-unix.ENODATA, 0)
  end
  update_current_time()
  xattr[name] = nil
  update_ctime(node)
end

function operations:readdir(path, fill)
  local node = get(path)
  update_current_time()
  for name in pairs(node.nodes) do
    fill(name)
  end
  update_atime(node)
end

function operations:create(path, mode)
  local parent_path, name = split(path)
  update_current_time()
  set(path, {
    attr = {
      st_mode = mode_file "0644";
      st_nlink = 1;
      st_uid = uid;
      st_gid = gid;
      st_atime = current_time;
      st_mtime = current_time;
      st_ctime = current_time;
      st_blocks = 0;
      st_size = 0
    };
    xattr = {};
    content = fuse.buffer();
  })
end

function operations:ftruncate(path, size)
  local node = get(path)
  local content = node.content
  content:resize(size)
  node.attr.st_size = #content
end

function operations:utimens(path, atime, mtime)
  local node = get(path)
  local attr = node.attr
  update_current_time()
  if not atime or atime.tv_nsec == unix.UTIME_NOW then
    atime = current_time
  end
  if atime.tv_nsec ~= unix.UTIME_OMIT then
    attr.st_atime = atime
  end
  if not mtime or mtime.tv_nsec == unix.UTIME_NOW then
    mtime = current_time
  end
  if mtime.tv_nsec ~= unix.UTIME_NOW then
    attr.st_mtime = mtime
  end
end

local result = fuse.main({ arg[0], ... }, operations)
assert(result == 0)
