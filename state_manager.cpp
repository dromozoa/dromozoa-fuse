// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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
  namespace {
    void impl_gc(lua_State* L) {
      check_state_manager(L, 1)->~state_manager();
    }
  }

  state_manager::~state_manager() {}

  state_manager* check_state_manager(lua_State* L, int arg) {
    return luaX_check_udata<state_manager>(L, arg, "dromozoa.fuse.state_manager");
  }

  void initialize_state_manager_main(lua_State*);

  void initialize_state_manager(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.fuse.state_manager");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      initialize_state_manager_main(L);
    }
    luaX_set_field(L, -2, "state_manager");
  }
}
