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

local fuse = require "dromozoa.fuse"

local buffer = fuse.buffer()
buffer:put(0, "foo")
buffer:put(3, "bar")
buffer:put(6, "baz")
assert(tostring(buffer) == "foobarbaz")
assert(buffer:get(0, 9) == "foobarbaz")
assert(buffer:get(3, 9) == "barbaz")
assert(buffer:get(6, 9) == "baz")
assert(#buffer == 9)
