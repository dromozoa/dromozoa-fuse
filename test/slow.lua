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

local operations = {}

function operations:getattr(path)
  if path == "/" then
    return {
      st_mode = unix.bor(unix.S_IFDIR, tonumber("0555", 8));
      st_nlink = 2;
    }
  elseif path == "/slow.txt" then
    return {
      st_mode = unix.bor(unix.S_IFREG, tonumber("0444", 8));
      st_nlink = 1;
      st_size = 4;
    }
  else
    error(-unix.ENOENT, 0)
  end
end

function operations:read(path)
  if path == "/slow.txt" then
    unix.nanosleep(2)
    return "foo\n"
  else
    error(-unix.ENOENT, 0)
  end
end

function operations:statfs(path)
  return {}
end

function operations:readdir(path, fill)
  if path == "/" then
    fill "."
    fill ".."
    fill "slow.txt"
  else
    error(-unix.ENOENT, 0)
  end
end

local result = fuse.main({ arg[0], ... }, fuse.state_manager.main(operations))
assert(result == 0)
