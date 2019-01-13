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

      luaX_string_reference get() const {
        return luaX_string_reference(buffer_.data(), buffer_.size());
      }

      luaX_string_reference get(size_t i, size_t n) const {
        size_t m = buffer_.size();
        if (m <= i) {
          return luaX_string_reference();
        }
        m -= i;
        return luaX_string_reference(&buffer_[i], std::min(m, n));
      }

      void put(size_t i, const luaX_string_reference& buffer) {
        size_t m = buffer_.size();
        size_t n = i + buffer.size();
        if (m < n) {
          buffer_.resize(n);
        }
        memcpy(&buffer_[i], buffer.data(), buffer.size());
      }

      size_t size() const {
        return buffer_.size();
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

    void impl_tostring(lua_State* L) {
      luaX_push(L, check_buffer(L, 1)->get());
    }

    void impl_len(lua_State* L) {
      luaX_push(L, check_buffer(L, 1)->size());
    }

    void impl_call(lua_State* L) {
      luaX_new<buffer>(L);
      luaX_set_metatable(L, "dromozoa.fuse.buffer");
    }

    void impl_get(lua_State* L) {
      buffer* self = check_buffer(L, 1);
      size_t i = luaX_check_integer<size_t>(L, 2);
      size_t n = luaX_check_integer<size_t>(L, 3);
      luaX_push(L, self->get(i, n));
    }

    void impl_put(lua_State* L) {
      buffer* self = check_buffer(L, 1);
      size_t i = luaX_check_integer<size_t>(L, 2);
      luaX_string_reference buffer = luaX_check_string(L, 3);
      self->put(i, buffer);
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
      luaX_set_field(L, -1, "__len", impl_len);
      luaX_set_field(L, -1, "__tostring", impl_tostring);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "get", impl_get);
      luaX_set_field(L, -1, "put", impl_put);
    }
    luaX_set_field(L, -2, "buffer");
  }
}
