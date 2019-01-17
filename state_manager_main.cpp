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

#include <dromozoa/bind/condition_variable.hpp>
#include <dromozoa/bind/mutex.hpp>

namespace dromozoa {
  namespace {
    class state_manager_main : public state_manager {
    public:
      state_manager_main(lua_State* L, int index) : state_(lua_newthread(L)) {
        lua_pushvalue(L, index);
        lua_xmove(L, state_, 1);
      }

      lua_State* open() {
        lock_guard<> lock(mutex_);
        while (!state_) {
          condition_.wait(lock);
        }
        lua_State* state = state_;
        state_ = 0;
        return state;
      }

      void close(lua_State* state) {
        lock_guard<> lock(mutex_);
        state_ = state;
        condition_.notify_one();
      }

    private:
      lua_State* state_;
      mutex mutex_;
      condition_variable condition_;
      state_manager_main(const state_manager_main&);
      state_manager_main& operator=(const state_manager_main&);
    };

    void impl_main(lua_State* L) {
      luaX_new<state_manager_main>(L, L, 1);
      luaX_set_metatable(L, "dromozoa.fuse.state_manager");
    }
  }

  void initialize_state_manager_main(lua_State* L) {
    luaX_set_field(L, -1, "main", impl_main);
  }
}
