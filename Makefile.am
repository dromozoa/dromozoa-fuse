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

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	.gitignore \
	README.md \
	bind \
	build_docs.lua \
	docs \
	dromozoa-fuse-*.rockspec \
	m4/update \
	test \
	test.sh
TESTS = \
	test/test_lua.sh \
	test/test_empty.sh \
	test/test_simple.sh \
	test/test_slow_main.sh \
	test/test_slow_pool.sh

luaexec_LTLIBRARIES = fuse.la

noinst_HEADERS = common.hpp

fuse_la_CPPFLAGS = -I$(top_srcdir)/bind
fuse_la_LDFLAGS = -module -avoid-version -shared
fuse_la_SOURCES = \
	buffer.cpp \
	convert.cpp \
	fill_dir.cpp \
	handle.cpp \
	main.cpp \
	managed_state.cpp \
	module.cpp \
	operations.cpp \
	state_manager.cpp \
	state_manager_main.cpp \
	state_manager_pool.cpp
