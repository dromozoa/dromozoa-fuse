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

size=`wc -c <foo/bar/test.txt`
size=`expr "X$size" : 'X *\([0-9][0-9]*\)$'`
case X$size in
  X9) ;;
  *) exit 1;;
esac

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

cat <<EOH >test.sh
#! /bin/sh -e
echo foo
echo bar
echo baz
EOH
chmod 755 test.sh
./test.sh

mkdir -p foo/bar/baz
echo 17 >foo/bar/baz/test.txt
echo 42 >test.txt
case X`cat test.txt` in
  X42) ;;
  *) exit 1;;
esac
mv foo/bar/baz/test.txt .
case X`cat test.txt` in
  X17) ;;
  *) exit 1;;
esac

mv foo/bar .
if test -d bar
then
  :
else
  exit 1
fi

if xattr >/dev/null 2>&1
then
  echo attr >attr.txt
  xattr -l attr.txt
  xattr -w dromozoa.foo 17 attr.txt
  xattr -w dromozoa.bar 42 attr.txt
  xattr -l attr.txt
  xattr -p dromozoa.foo attr.txt
  xattr -p dromozoa.bar attr.txt
  if xattr -p dromozoa.baz attr.txt
  then
    exit 1
  fi
  xattr -d dromozoa.bar attr.txt
  xattr -l attr.txt
  xattr -p dromozoa.foo attr.txt
  if xattr -p dromozoa.bar attr.txt
  then
    exit 1
  fi
elif attr -l / >/dev/null 2>&1
then
  echo attr >attr.txt
  attr -l attr.txt
  attr -s dromozoa.foo -V 17 attr.txt
  attr -s dromozoa.bar -V 42 attr.txt
  attr -l attr.txt
  attr -g dromozoa.foo attr.txt
  attr -g dromozoa.bar attr.txt
  if attr -g dromozoa.baz attr.txt
  then
    exit 1
  fi
  attr -r dromozoa.bar attr.txt
  attr -l attr.txt
  attr -g dromozoa.foo attr.txt
  if attr -g dromozoa.bar attr.txt
  then
    exit 1
  fi
fi

touch touch.txt
touch touch.txt
