# Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

mount_point=$1

cd "$mount_point"

if test -d foo/bar
then
  exit 1
fi

mkdir foo
mkdir foo/bar

if test -d foo/bar
then
  :
else
  exit 1
fi

if test -f foo/bar/test.txt
then
  exit 1
fi

printf foo >foo/bar/test.txt
printf bar >>foo/bar/test.txt
printf baz >>foo/bar/test.txt
case X`cat foo/bar/test.txt` in
  Xfoobarbaz) ;;
  *) exit 1;;
esac

if test -f foo/bar/test.txt
then
  :
else
  exit 1
fi

rm foo/bar/test.txt

if test -f foo/bar/test.txt
then
  exit 1
fi

rmdir foo/bar
rmdir foo

if test -d foo/bar
then
  exit 1
fi
