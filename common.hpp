// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-fuse.
//
// dromozoa-fuse is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-fuse is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-fuse.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#define FUSE_USE_VERSION 28
#include <fuse.h>

#include <dromozoa/bind.hpp>

namespace dromozoa {
  void new_file_info(lua_State* L, fuse_file_info* that);
  void set_file_info(lua_State* L, int arg, fuse_file_info* that);
  // void new_fill_dir(lua_State* L, void* that, fuse_fill_dir_t f);
}

#endif
