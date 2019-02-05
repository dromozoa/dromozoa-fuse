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

local class = {}
local metatable = { __index = class }

function class:get(i, n)
  local m = #self
  if not i then
    i = 0
  end
  if not n then
    n = m
  end
  if m <= i then
    return ""
  end
  if n > m then
    n = m
  end

  return table.concat(self, "", i + 1, n)
end

function class:put(i, buffer)
  for j = 1, #buffer do
    self[i + j] = buffer:sub(j, j)
  end
  return self
end

function class:resize(n)
  local m = #self
  for i = m + 1, n do
    self[i] = "\0"
  end
  for i = n + 1, m do
    self[i] = nil
  end
  return self
end

function metatable:__tostring()
  return self:get()
end

return setmetatable(class, {
  __call = function ()
    return setmetatable({}, metatable)
  end;
})
