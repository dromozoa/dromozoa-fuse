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

#include <iostream>

namespace dromozoa {
  namespace {
    class file_info_handle : public handle {
    public:
      explicit file_info_handle(fuse_file_info* ptr)
        : ptr_(ptr) {}

      void reset() {
        ptr_ = 0;
      }

      fuse_file_info* get() const {
        if (!ptr_) {
          luaX_throw_failure("out of scope");
        }
        return ptr_;
      }

    private:
      fuse_file_info* ptr_;
      file_info_handle(const file_info_handle&);
      file_info_handle& operator=(const file_info_handle&);
    };

    fuse_file_info* check_file_info(lua_State* L, int arg) {
      return luaX_check_udata<file_info_handle>(L, arg, "dromozoa.fuse.file_info")->get();
    }

    void impl_index(lua_State* L) {
      std::cout << "-\n";
      fuse_file_info* self = check_file_info(L, 1);
      luaX_string_reference buffer = luaX_check_string(L, 2);
      std::string key(buffer.data(), buffer.size());
      std::cout << "?" << key << "\n";
      if (key == "flags") {
        luaX_push(L, self->flags);
      }
    }

    void impl_newindex(lua_State* L) {
    }
  }

  handle* new_file_info_handle(lua_State* L, fuse_file_info* ptr) {
    file_info_handle* self = luaX_new<file_info_handle>(L, ptr);
    luaX_set_metatable(L, "dromozoa.fuse.file_info");
    return self;
  }

  void initialize_file_info(lua_State* L) {
    luaL_newmetatable(L, "dromozoa.fuse.file_info");
    luaX_set_field(L, -1, "__index", impl_index);
    luaX_set_field(L, -1, "__newindex", impl_newindex);
    lua_pop(L, 1);
  }
}
