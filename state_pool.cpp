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

#include <sstream>

namespace dromozoa {
  namespace {
    class scoped_state {
    public:
      explicit scoped_state(lua_State* ptr) : ptr_(ptr) {}

      ~scoped_state() {
        if (ptr_) {
          lua_close(ptr_);
        }
      }

      lua_State* get() const {
        return ptr_;
      }

      lua_State* release() {
        lua_State* ptr = ptr_;
        ptr_ = 0;
        return ptr;
      }

    private:
      lua_State* ptr_;
      scoped_state(const scoped_state&);
      scoped_state& operator=(const scoped_state&);
    };

    lua_State* construct(const std::string& chunk, const std::string& name) {
      scoped_state state(luaL_newstate());
      if (lua_State* L = state.get()) {
        luaL_openlibs(L);
        int result = luaL_loadbuffer(L, chunk.data(), chunk.size(), name.c_str());
        if (result == 0) {
          if (lua_pcall(L, 0, 1, 0) == 0) {
            return state.release();
          } else {
            DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
          }
        } else {
          std::ostringstream out;
          out << "could not luaL_loadbuffer: error number " << result;
          DROMOZOA_UNEXPECTED(out.str());
        }
      } else {
        DROMOZOA_UNEXPECTED("could not luaL_newstate");
      }
      return 0;
    }
  }

  state_pool::state_pool(size_t start_states, size_t max_idle_states, const std::string& chunk, const std::string& name)
    : max_idle_states_(max_idle_states),
      chunk_(chunk),
      name_(name),
      active_states_() {
    for (size_t i = 0; i < start_states; ++i) {
      scoped_state state(construct(chunk_, name_));
      if (lua_State* L = state.get()) {
        idle_states_.push_back(L);
        state.release();
      } else {
        break;
      }
    }
  }

  state_pool::~state_pool() {
    while (true) {
      lua_State* L = 0;
      {
        lock_guard<> lock(mutex_);
        if (idle_states_.empty()) {
          break;
        }
        L = idle_states_.front();
        idle_states_.pop_front();
      }
      lua_close(L);
    }

    {
      lock_guard<> lock(mutex_);
      if (active_states_ > 0) {
        DROMOZOA_UNEXPECTED("there are active states");
      }
    }
  }

  lua_State* state_pool::open() {
    {
      lock_guard<> lock(mutex_);
      if (!idle_states_.empty()) {
        lua_State* L = idle_states_.front();
        idle_states_.pop_front();
        return L;
      }
    }
    return 0;
  }
}
