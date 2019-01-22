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
  namespace {
    class fill_dir : public handle {
    public:
      fill_dir(fuse_fill_dir_t function, void* buffer)
        : function_(function),
          buffer_(buffer) {}

      virtual void reset() {
        function_ = 0;
        buffer_ = 0;
      }

      int operator()(const char* name, const struct stat* buffer, off_t offset) {
        if (function_ && buffer_) {
          return function_(buffer_, name, buffer, offset);
        } else {
          luaX_throw_failure("out of scope");
          return 1;
        }
      }

    public:
      fuse_fill_dir_t function_;
      void* buffer_;
      fill_dir(const fill_dir&);
      fill_dir& operator=(const fill_dir&);
    };

    void impl_call(lua_State* L) {
      fill_dir* self = luaX_check_udata<fill_dir>(L, 1, "dromozoa.fuse.fill_dir");
      luaX_string_reference name = luaX_check_string(L, 2);
      const struct stat* buffer_ptr = 0;
      struct stat buffer = {};
      if (convert(L, 3, &buffer)) {
        buffer_ptr = &buffer;
      }
      off_t offset = luaX_opt_integer<off_t>(L, 4, 0);
      int result = (*self)(name.data(), buffer_ptr, offset);
      luaX_push(L, result);
    }
  }

  handle* new_fill_dir(lua_State* L, fuse_fill_dir_t function, void* buffer) {
    fill_dir* self = luaX_new<fill_dir>(L, function, buffer);
    luaX_set_metatable(L, "dromozoa.fuse.fill_dir");
    return self;
  }

  void initialize_fill_dir(lua_State* L) {
    luaL_newmetatable(L, "dromozoa.fuse.fill_dir");
    luaX_set_field(L, -1, "__call", impl_call);
    lua_pop(L, 1);
  }
}
