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

t=`lua -e "local unix = require 'dromozoa.unix' print(unix.clock_gettime(unix.CLOCK_MONOTONIC):tostring())"`

cat "$mount_point/slow1.txt" >test-slow1.txt &
pid1=$!
cat "$mount_point/slow2.txt" >test-slow2.txt &
pid2=$!
cat "$mount_point/slow3.txt" >test-slow3.txt &
pid3=$!
cat "$mount_point/slow4.txt" >test-slow4.txt &
pid4=$!

wait "$pid1" "$pid2" "$pid3" "$pid4"
t=`lua -e "local unix = require 'dromozoa.unix' print(math.floor((unix.clock_gettime(unix.CLOCK_MONOTONIC):tonumber() - $t) * 1000))"`

tid1=`cat test-slow1.txt`
tid2=`cat test-slow2.txt`
tid3=`cat test-slow3.txt`
tid4=`cat test-slow4.txt`
rm test-slow1.txt test-slow2.txt test-slow3.txt test-slow4.txt

echo "[[[[$tid1]]]]"
echo "[[[[$tid2]]]]"
echo "[[[[$tid3]]]]"
echo "[[[[$tid4]]]]"
echo "[[[[$t]]]]"

if test "$t" -lt 200 -o 400 -lt "$t"
then
  exit 1
fi
