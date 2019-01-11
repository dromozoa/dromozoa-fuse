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

local function update_atime(node)
  node.attr.st_atime = current_time
end

local function update_mtime(node)
  node.attr.st_mtime = current_time
end

local function update_ctime(node)
  node.attr.st_ctime = current_time
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
  local this = get(oldpath)
  local attr = this.attr
  attr.st_nlink = attr.st_nlink + 1
  attr.st_ctime = current_time;
  set(newpath, this)
end

link("/", "/.")

local operations = {}

function operations:getattr(path)
  return get(path).attr
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
  return 0
end

function operations:statfs(path)
  return vfs
end

function operations:readdir(path, fill)
  local this = get(path)
  for name in pairs(this.nodes) do
    fill(name)
  end
  return 0
end

local result = fuse.main({ arg[0], ... }, operations)
assert(result == 0)

--[====[


local root = {
  {
    st_mode = unix.bor(unix.S_IFDIR, tonumber("0755", 8));
    st_nlink = 1;
    st_uid = uid;
    st_gid = gid;
    st_atime = now;
    st_mtime = now;
    st_ctime = now;
    st_blocks = 0;
  }
}

local function parse(path)
  if path == "/" then
    return nil, "/"
  else
    local parent_path, name = path:match "^(.*)/([^/]+)$"
    if #parent_path == 0 then
      return "/", name
    else
      return parent_path, name
    end
  end
end

local function get(path)
  if path == "/" then
    return root
  else
    local items = {}
    for item in path:gmatch "/([^/]+)" do
      items[#items + 1] = item
    end
    local this = root
    local n = #items
    for i = 1, n - 1 do
      this = this[items[i]]
      if not this then
        return
      end
    end
    return this[items[n]]
  end
end

local function set(path, data)
  local items = {}
  for item in path:gmatch "/([^/]+)" do
    items[#items + 1] = item
  end
  local this = root
  local n = #items
  for i = 1, n - 1 do
    this = this[items[i]]
  end
  if data then
    this[items[n]] = { data }
  else
    this[items[n]] = nil
  end
end

local function unlink(path)
  local data = get(path)[1]
  data.st_nlink = data.st_nlink - 1
  data.st_atime = now
  data.st_mtime = now
end

local function link(source, result)
  local data = get(source)[1]
  data.st_nlink = data.st_nlink + 1
  data.st_atime = now
  data.st_mtime = now
  set(result, data)
end

root[".."] = {}
link("/", "/.")

local ops = {}

function ops:getattr(path)
  local this = get(path)
  if not this then
    return -unix.ENOENT
  end
  return this[1]
end

function ops:readlink(path)
  local this = get(path)
  if not this then
    return -unix.ENOENT
  end
  if unix.band(this[1].st_mode, unix.S_IFLNK) == 0 then
    return -unix.EINVAL
  end
  return this[1][1]
end

function ops:mkdir(path)
  local parent_path, name = parse(path)
  if not parent_path then
    return -unix.EEXIST
  end
  local parent = get(parent_path)
  if not parent then
    return -unix.ENOENT
  end
  if unix.band(parent[1].st_mode, unix.S_IFDIR) == 0 then
    return -unix.ENOTDIR
  end
  if get(path) then
    return -unix.EEXIST
  end

  now = os.time()
  set(path, {
    st_mode = unix.bor(unix.S_IFDIR, tonumber("0755", 8));
    st_nlink = 1;
    st_uid = uid;
    st_gid = gid;
    st_atime = now;
    st_mtime = now;
    st_ctime = now;
    st_blocks = 0;
  })

  link(parent_path, path .. "/..")
  link(path, path .. "/.")

  return 0
end

function ops:rmdir(path)
  local this = get(path)
  if unix.band(this[1].st_mode, unix.S_IFDIR) == 0 then
    return -unix.ENOTDIR
  end
  for name in pairs(this) do
    if type(name) == "string" and name ~= "." and name ~= ".." then
      return -unix.ENOTEMPTY
    end
  end

  now = os.time()
  unlink(path .. "/..")
  set(path)
  return 0
end

function ops:symlink(source, result)
  print("!", source, result)

  local parent_path, name = parse(result)
  if not parent_path then
    return -unix.EEXIST
  end
  local parent = get(parent_path)
  if not parent then
    return -unix.ENOENT
  end
  if unix.band(parent[1].st_mode, unix.S_IFDIR) == 0 then
    return -unix.ENOTDIR
  end
  if get(result) then
    return -unix.EEXIST
  end

  now = os.time()
  set(result, {
    st_mode = unix.bor(unix.S_IFLNK, tonumber("0777", 8));
    st_nlink = 1;
    st_uid = uid;
    st_gid = gid;
    st_atime = { tv_sec = now, tv_nsec = 123456789 };
    st_mtime = { tv_sec = now, tv_nsec = 424242424 };
    st_ctime = { tv_sec = now, tv_nsec = 696969696 };
    st_blocks = 0;
    st_size = #source;
    source;
  })

  -- TODO update directory entry?

  return 0
end

function ops:read(path, size, offset, info)
  return -unix.ENOTSUP
end

function ops:write(path, data, offset, info)
  return -unix.ENOTSUP
end

function ops:statfs(path)
  return {
    f_bsize = 512;
    f_frsize = 512;
    f_blocks = 1024;
    f_bfree = 1024;
    f_bavail = 1024;
    f_files = 1024;
    f_ffree = 1000;
    f_namemax = 256;
  }
end

function ops:opendir()
  return 0
end

function ops:readdir(path, fill, offset, info)
  local this = get(path)
  for name, value in pairs(this) do
    if type(name) == "string" then
      fill(name)
    end
  end
  return 0
end

function ops:closedir()
  return 0
end

]====]
