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
  nodes = {
    [".."] = true;
  };
}

local function split(path)
  if path == "/" then
    error(-unix.EEXIST)
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
      error(-unix.ENOENT)
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
    error(-unix.EEXIST)
  end
  local this = root
  for i = 1, n - 1 do
    this = this.nodes[names[i]]
    if not this then
      error(-unix.ENOENT)
    end
  end
  local name = names[n]
  local nodes = this.nodes
  if node then
    if nodes[name] then
      error(-unix.EEXIST)
    end
  else
    if not nodes[name] then
      error(-unix.ENOENT)
    end
  end
  nodes[name] = node
end

local function link(oldpath, newpath)
  set(newpath, increment_link(get(oldpath)))
end

link("/", "/.")

local operations = {}

function operations:getattr(path)
  return get(path).attr
end

function operations:unlink(path)
  local node = get(path)
  local parent_path, name = split(path)
  local parent_node = get(parent_path)
  update_current_time()
  set(path)
  decrement_link(node)
  update_mtime(parent_node)
end

function operations:rmdir(path)
  local node = get(path)
  local parent_path, name = split(path)
  local parent_node = get(parent_path)

  if unix.band(node.attr.st_mode, unix.S_IFDIR) == 0 then
    error(-unix.ENOTDIR)
  end
  if not is_empty_dir(node) then
    error(-unix.ENOTEMPTY)
  end

  update_current_time()
  set(path .. "/..")
  set(path .. "/.")
  set(path)
  decrement_link(node)
  decrement_link(node)
  decrement_link(parent_node)
  update_mtime(parent_node)
end

function operations:mkdir(path)
  local parent_path, name = split(path)
  local parent_node = get(parent_path)
  update_current_time()
  set(path, {
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
    nodes = {};
  })
  link(parent_path, path .. "/..")
  link(path, path .. "/.")
  update_mtime(parent_node)
end

function operations:read(path, size, offset)
  local node = get(path)
  local content = node.content
  local buffer = {}
  for i = 1, size do
    buffer[i] = content[offset + i]
  end
  print(("READ %s %d %d %q"):format(path, size, offset, table.concat(buffer)))
  return table.concat(buffer)
end


function operations:write(path, buffer, offset)
  local node = get(path)
  local content = node.content
  local n = #buffer
  for i = 1, n do
    content[offset + i] = buffer:sub(i, i)
  end
  print(("WRITE %s %d %d %q"):format(path, #buffer, offset, table.concat(content)))
  node.attr.st_size = #content
end

function operations:statfs(path)
  return vfs
end

function operations:readdir(path, fill)
  local this = get(path)
  for name in pairs(this.nodes) do
    fill(name)
  end
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
    };
    content = {};
  })
end

function operations:ftruncate(path, size)
  local node = get(path)
  local content = node.content
  local n = #content
  if n < size then
    for i = n + 1, size do
      content[i] = "\0"
    end
  else
    for i = size + 1, n do
      content[i] = nil
    end
  end
  node.attr.st_size = #content
end

local result = fuse.main({ arg[0], ... }, operations)
assert(result == 0)
