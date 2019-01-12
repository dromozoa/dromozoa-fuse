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

#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <vector>

namespace dromozoa {
  namespace {
    class buffer {
    public:
      buffer() {}

      luaX_string_reference read(std::size_t i, std::size_t n) const {
        std::size_t m = buffer_.size();
        if (m <= i) {
          return luaX_string_reference();
        }
        m -= i;
        return luaX_string_reference(&buffer_[i], std::min(m, n));
      }

      void write(std::size_t i, const luaX_string_reference& buffer) {
        std::size_t m = buffer_.size();
        std::size_t n = i + buffer.size();
        if (m < n) {
          buffer_.resize(n);
        }
        memcpy(&buffer_[i], buffer.data(), buffer.size());
      }

    private:
      std::vector<char> buffer_;
      buffer(const buffer&);
      buffer& operator=(const buffer&);
    };

    buffer* check_buffer(lua_State* L, int arg) {
      return luaX_check_udata<buffer>(L, arg, "dromozoa.fuse.buffer");
    }

    void impl_gc(lua_State* L) {
      check_buffer(L, 1)->~buffer();
    }

    void impl_call(lua_State* L) {
      luaX_new<buffer>(L);
      luaX_set_metatable(L, "dromozoa.fuse.buffer");
    }

    void impl_read(lua_State* L) {
      buffer* self = check_buffer(L, 1);
      std::size_t i = luaX_check_integer<std::size_t>(L, 2);
      std::size_t n = luaX_check_integer<std::size_t>(L, 3);
      luaX_push(L, self->read(i, n));
    }

    void impl_write(lua_State* L) {
      buffer* self = check_buffer(L, 1);
      std::size_t i = luaX_check_integer<std::size_t>(L, 2);
      luaX_string_reference buffer = luaX_check_string(L, 3);
      self->write(i, buffer);
      luaX_push_success(L);
    }
  }

  void initialize_buffer(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.fuse.buffer");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "read", impl_read);
      luaX_set_field(L, -1, "write", impl_write);
    }
    luaX_set_field(L, -2, "buffer");
  }
}
