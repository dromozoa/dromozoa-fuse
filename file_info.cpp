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

#include "common.hpp"

namespace dromozoa {
  int new_file_info(lua_State* L, fuse_file_info* that) {
    lua_newtable(L);
    int index = lua_gettop(L);
    luaX_set_field(L, index, "flags", that->flags);
    luaX_set_field(L, index, "writepage", that->writepage);
    luaX_set_field(L, index, "direct_io", that->direct_io);
    luaX_set_field(L, index, "keep_cache", that->keep_cache);
    luaX_set_field(L, index, "flush", that->flush);
    luaX_set_field(L, index, "nonseekable", that->nonseekable);
    luaX_set_field(L, index, "flock_release", that->flock_release);
    luaX_set_field(L, index, "fh", that->fh);
    luaX_set_field(L, index, "lock_owner", that->lock_owner);
    return index;
  }

  void set_file_info(lua_State* L, int arg, fuse_file_info* that) {
    luaL_checktype(L, arg, LUA_TTABLE);
    that->flags = luaX_opt_integer_field(L, arg, "flags", that->flags);
    that->writepage = luaX_opt_integer_field(L, arg, "writepage", that->writepage);
    that->direct_io = luaX_opt_integer_field(L, arg, "direct_io", that->direct_io);
    that->keep_cache = luaX_opt_integer_field(L, arg, "keep_cache", that->keep_cache);
    that->flush = luaX_opt_integer_field(L, arg, "flush", that->flush);
    that->nonseekable = luaX_opt_integer_field(L, arg, "nonseekable", that->nonseekable);
    that->flock_release = luaX_opt_integer_field(L, arg, "nonseekable", that->flock_release);
    that->fh = luaX_opt_integer_field(L, arg, "fh", that->fh);
    that->lock_owner = luaX_opt_integer_field(L, arg, "lock_owner", that->lock_owner);
  }
}
