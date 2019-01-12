-- Copyright (C) 2018,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local operations = {}

function operations:getattr(path)
  if path == "/" then
    return {
      st_mode = unix.bor(unix.S_IFDIR, tonumber("0755", 8));
      st_nlink = 2;
    }
  else
    error(-unix.ENOENT)
  end
end

function operations:statfs(path)
  return {}
end

function operations:readdir(path, fill)
  if path == "/" then
    fill "."
    fill ".."
  else
    return -unix.ENOENT
  end
end

local result = fuse.main({ arg[0], ... }, operations)
assert(result == 0)
