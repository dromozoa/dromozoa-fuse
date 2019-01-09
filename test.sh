#! /bin/sh -e

# Copyright (C) 2018,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
#
# This file is part of dromozoa-fuse.
#
# dromozoa-fuse is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dromozoa-fuse is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dromozoa-fuse.  If not, see <http://www.gnu.org/licenses/>.

root=`pwd`

PATH=$root/tool:$PATH
export PATH

mountpoint=$root/mount

mkdir -p "$mountpoint"
case X$# in
  X0) lua test/test.lua "$mountpoint" -d -s &;;
  *) "$@" test/test.lua "$mountpoint" -d -s &;;
esac
pid=$!

./test/test.sh "$mountpoint"

wait "$pid"

rmdir "$mountpoint"
