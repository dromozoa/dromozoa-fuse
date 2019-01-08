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

local PATH = os.getenv "PATH"

local fuse_script, test_script = ...

local process1 = unix.process()
local process2 = unix.process()

process1:forkexec(PATH, { fuse_script, "-d", "-s" })
process2:forkexec(PATH, { test_script })

local pid, reason, status = assert(unix.wait())
assert(pid == process1[1] or process2[1])
assert(reason == "exit")
assert(status == 0)

local pid, reason, status = assert(unix.wait())
assert(pid == process1[1] or process2[1])
assert(reason == "exit")
assert(status == 0)
