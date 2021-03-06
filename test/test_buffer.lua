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

local buffer = require "test.buffer"

local b = buffer()
b:put(0, "foo")
b:put(3, "bar")
b:put(6, "baz")
assert(tostring(b) == "foobarbaz")
assert(b:get(0, 9) == "foobarbaz")
assert(b:get(3, 9) == "barbaz")
assert(b:get(6, 9) == "baz")
assert(#b == 9)
b:resize(3)
assert(tostring(b) == "foo")
b:resize(6)
assert(tostring(b) == "foo\0\0\0")
