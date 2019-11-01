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

m4_include([version.m4])
AC_INIT([dromozoa-fuse], DROMOZOA_FUSE_VERSION, [moyu@dromozoa.com], [], [https://github.com/dromozoa/dromozoa-fuse/])
AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])

AC_PROG_CXX

AC_CHECK_MEMBERS([struct stat.st_atim])
AC_CHECK_MEMBERS([struct stat.st_atimespec])
AC_CHECK_MEMBERS([struct stat.st_ctim])
AC_CHECK_MEMBERS([struct stat.st_ctimespec])
AC_CHECK_MEMBERS([struct stat.st_mtim])
AC_CHECK_MEMBERS([struct stat.st_mtimespec])

AX_PTHREAD([], [AC_MSG_ERROR([could not find pthread])])
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
LIBS="$LIBS $PTHREAD_LIBS"
AC_SEARCH_LIBS([pthread_create], [pthread])

AC_SEARCH_LIBS([fuse_main], [osxfuse fuse], [], [AC_MSG_ERROR([could not find fuse])])
AC_CHECK_HEADER([osxfuse/fuse.h], [AC_DEFINE(HAVE_OSXFUSE_FUSE_H, 1, [Define to 1 if you have the <osxfuse/fuse.h> header file.])], [], [
AC_INCLUDES_DEFAULT
#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 28
])

AX_PROG_LUA([5.1], [], [], [AC_MSG_ERROR([could not find lua])])
AX_LUA_HEADERS([], [AC_MSG_ERROR([could not find lua])])

AC_OUTPUT
