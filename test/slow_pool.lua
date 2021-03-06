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
local multi = require "dromozoa.multi"
local fuse = require "dromozoa.fuse"

if arg then
  local handle = io.open(arg[0])
  local chunk = handle:read "*a"
  handle:close()
  local result = fuse.main({ arg[0], ... }, fuse.state_manager.pool(4, 4, 8, chunk, arg[0]))
  print("result", result)
  assert(result == 0)
  return
end

local operations = {}

function operations:getattr(path)
  if path == "/" then
    return {
      st_mode = unix.bor(unix.S_IFDIR, tonumber("0555", 8));
      st_nlink = 2;
    }
  elseif path:find "/slow%d.txt" then
    return {
      st_mode = unix.bor(unix.S_IFREG, tonumber("0444", 8));
      st_nlink = 1;
      st_size = 64;
    }
  else
    error(-unix.ENOENT, 0)
  end
end

function operations:read(path)
  if path:find "/slow%d.txt" then
    unix.nanosleep(0.2)
    return ("%-63s\n"):format(multi.this_thread_id() .. " " .. multi.this_state_id())
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
    for i = 0, 9 do
      fill(("slow%d.txt"):format(i))
    end
  else
    error(-unix.ENOENT, 0)
  end
end

return operations
