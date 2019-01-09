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

dromozoa_umount() {
  if fusermount -V >/dev/null 2>&1
  then
    fusermount -u "$1"
  else
    umount "$1"
  fi
}

root=`pwd`/mount

mkdir -p "$root"

case X$# in
  X0) lua test/test.lua "$root" -d -s &;;
  *) "$@" test/test.lua "$root" -d -s &;;
esac
pid=$!

sleep 1

if ./test/test.sh "$root"
then
  test_result=OK
else
  test_result=NG
fi

sleep 1

dromozoa_umount "$root"

if wait "$pid"
then
  fuse_result=OK
else
  fuse_result=NG
fi

rmdir "$root"

result=$fuse_result/$test_result
echo "$result"
case X$result in
  XOK/OK) ;;
  *) exit 1;;
esac
