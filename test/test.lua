-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local fuse = require "dromozoa.fuse"
local unix = require "dromozoa.unix"

local args = {}
for i = 0, #arg do
  args[#args + 1] = arg[i]
end

local uid = unix.getuid();
local gid = unix.getgid();
local t = os.time()

local data = "hello world!\n"

local ops = {}
local dirs = {}

function ops:getattr(path)
  if path == "/" then
    return {
      st_mode = unix.bor(unix.S_IFDIR, tonumber("0755", 8));
      st_nlink = 2;
      st_uid = uid;
      st_gid = gid;
      st_atime = t;
      st_mtime = t;
      st_ctime = t;
      st_blocks = 0;
    }
  elseif path == "/test.txt" then
    return {
      st_mode = unix.bor(unix.S_IFREG, tonumber("0644", 8));
      st_nlink = 1;
      st_uid = uid;
      st_gid = gid;
      st_atime = t;
      st_mtime = t;
      st_ctime = t;
      st_size = #data;
      st_blocks = 0;
    }
  elseif path == "/link.txt" then
    return {
      st_mode = unix.bor(unix.S_IFLNK, tonumber("0777", 8));
      st_nlink = 1;
      st_uid = uid;
      st_gid = gid;
      st_atime = t;
      st_mtime = t;
      st_ctime = t;
      st_size = 8;
      st_blocks = 0;
    }
  elseif dirs[path] then
    return dirs[path]
  else
    return -unix.ENOENT
  end
end

function ops:readlink(path)
  if path == "/link.txt" then
    return "test.txt"
  else
    return -unix.ENOENT
  end
end

function ops:mkdir(path, mode)
  print("mkdir", path, ("%04o"):format(mode))
  local t = os.time()
  dirs[path] = {
    st_mode = unix.bor(unix.S_IFDIR, mode);
    st_nlink = 2;
    st_uid = uid;
    st_gid = gid;
    st_atime = t;
    st_mtime = t;
    st_ctime = t;
    st_blocks = 0;
  }
  return 0
end

function ops:getxattr(path, name)
  print("getattr", path, name)
  return -unix.ENOTSUP
end

function ops:readdir(path, fill, offset, fi)
  print("readdir", path, fill, offset, fi)
  local r = fill(".", nil, 0)
  print("readdir", r)
  local r = fill("..", nil, 0)
  print("readdir", r)
  local r = fill("test.txt", nil, 0)
  print("readdir", r)
  local r = fill("link.txt", nil, 0)
  print("readdir", r)
  return 0
end

function ops:open(path, info)
  if path == "/test.txt" then
    return 0
  else
    return -unix.ENOENT
  end
end

function ops:read(path, size, offset, info)
  if path == "/" then
    return -unix.EISDIR
  elseif path == "/test.txt" then
    print(size, offset, info)
    if offset < #data then
      local i = offset + 1
      local j = math.min(#data - offset, size)
      return data:sub(i, j)
    end
    return 0
  else
    return -unix.ENOENT
  end
end

local result = fuse.main(args, ops)
print(result)
