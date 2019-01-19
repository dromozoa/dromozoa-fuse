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

extern "C" {
#include <lualib.h>
}

#include <list>
#include <sstream>
#include <stdexcept>

#include <dromozoa/bind/condition_variable.hpp>
#include <dromozoa/bind/mutex.hpp>

namespace dromozoa {
  namespace {
    class scoped_state {
    public:
      explicit scoped_state(lua_State* state) : state_(state) {}

      ~scoped_state() {
        if (state_) {
          lua_close(state_);
        }
      }

      lua_State* get() const {
        return state_;
      }

      lua_State* release() {
        lua_State* state = state_;
        state_ = 0;
        return state;
      }

    private:
      lua_State* state_;
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
            throw std::runtime_error(lua_tostring(L, -1));
          }
        } else {
          std::ostringstream out;
          out << "could not luaL_loadbuffer: error number " << result;
          throw std::runtime_error(out.str());
        }
      } else {
        throw std::runtime_error("could not luaL_newstate");
      }
    }

    class state_manager_pool : public state_manager {
    public:
      state_manager_pool(size_t start_states, size_t max_states, size_t max_idle_states, const std::string& chunk, const std::string& name)
        : max_states_(max_states),
          max_idle_states_(max_idle_states),
          chunk_(chunk),
          name_(name),
          active_states_() {
        for (size_t i = 0; i < start_states; ++i) {
          scoped_state state(construct(chunk_, name_));
          idle_states_.push_back(state.get());
          state.release();
        }
      }

      ~state_manager_pool() {
        while (!idle_states_.empty()) {
          scoped_state state(idle_states_.front());
          idle_states_.pop_front();
        }
      }

      lua_State* open() {
        {
          lock_guard<> lock(mutex_);
          while (idle_states_.empty() && active_states_ >= max_states_) {
            condition_.wait(lock);
          }
          ++active_states_;
          if (!idle_states_.empty()) {
            scoped_state state(idle_states_.front());
            idle_states_.pop_front();
            return state.release();
          }
        }
        return construct(chunk_, name_);
      }

      void close(lua_State* L) {
        scoped_state state(L);
        {
          lock_guard<> lock(mutex_);
          --active_states_;
          if (idle_states_.size() < max_idle_states_) {
            idle_states_.push_back(state.get());
            state.release();
          }
        }
      }

    private:
      size_t max_states_;
      size_t max_idle_states_;
      std::string chunk_;
      std::string name_;
      mutex mutex_;
      condition_variable condition_;
      size_t active_states_;
      std::list<lua_State*> idle_states_;

      state_manager_pool(const state_manager_pool&);
      state_manager_pool& operator=(const state_manager_pool&);
    };

    void impl_pool(lua_State* L) {
      size_t start_states = luaX_check_integer<size_t>(L, 1);
      size_t max_states = luaX_check_integer<size_t>(L, 2);
      size_t max_idle_states = luaX_check_integer<size_t>(L, 3);
      luaX_string_reference chunk = luaX_check_string(L, 4);
      luaX_string_reference name = luaX_check_string(L, 5);
      luaX_new<state_manager_pool>(L, start_states, max_states, max_idle_states, std::string(chunk.data(), chunk.size()), std::string(name.data(), name.size()));
      luaX_set_metatable(L, "dromozoa.fuse.state_manager");
    }
  }

  void initialize_state_manager_pool(lua_State* L) {
    luaX_set_field(L, -1, "pool", impl_pool);
  }
}
