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

local ops = {}
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
  else
    return -unix.ENOENT
  end
end
function ops:getxattr(path, name)
  print("getattr", path, name)
  return -unix.ENOTSUP
end

local result = fuse.main(args, ops)
print(result)
