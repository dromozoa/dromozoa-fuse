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

#include <errno.h>
#include <string.h>

#include <iostream>

// int (*getattr) (const char *, struct stat *);
// int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
// int (*open) (const char *, struct fuse_file_info *);
// int (*read) (const char *, char *, size_t, off_t, struct fuse_file_info *);

#define DROMOZOA_SET_OPERATION(name) \
  operations.name = name \
  /**/

namespace dromozoa {
  namespace {
    void* init(struct fuse_conn_info*) {
      return fuse_get_context()->private_data;
    }

    void destroy(void* userdata) {
      scoped_ptr<luaX_reference<> > self(static_cast<luaX_reference<>*>(userdata));
    }

    int getattr(const char* path, struct stat* buf) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->get_field(L) == LUA_TNIL) {
        return -ENOSYS;
      }
      if (luaX_get_field(L, -1, "getattr") == LUA_TNIL) {
        return -ENOSYS;
      }
      lua_pushvalue(L, -2);
      luaX_push(L, path);
      if (lua_pcall(L, 2, 1, 0) == 0) {
        if (lua_istable(L, -1)) {
          convert(L, -1, buf);
          return 0;
        } else if (luaX_is_integer(L, -1)) {
          return lua_tointeger(L, -1);
        }
        return -ENOSYS;
      } else {
        return -ENOSYS;
      }
    }

    int getxattr(const char* path, const char* name, char* value, size_t size) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->get_field(L) == LUA_TNIL) {
        return -ENOSYS;
      }
      if (luaX_get_field(L, -1, "getxattr") == LUA_TNIL) {
        return -ENOSYS;
      }
      lua_pushvalue(L, -2);
      luaX_push(L, path, name);
      if (lua_pcall(L, 3, 1, 0) == 0) {
        if (luaX_is_integer(L, -1)) {
          return lua_tointeger(L, -1);
        } else if (luaX_string_reference result = luaX_to_string(L, -1)) {
          if (result.size() < size) {
            memcpy(value, result.data(), result.size());
            value[result.size()] = '\0';
            return result.size();
          } else {
            return -ERANGE;
          }
        } else {
          return -ENOTSUP;
        }
      } else {
        return -ENOSYS;
      }
    }

    int readdir(const char* path, void* buffer, fuse_fill_dir_t function, off_t offset, struct fuse_file_info* file_info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      int file_info_index = convert(L, file_info);
      if (self->get_field(L) == LUA_TNIL || luaX_get_field(L, -1, "readdir") == LUA_TNIL) {
        return -ENOSYS;
      }
      lua_pushvalue(L, -2);
      luaX_push(L, path);
      scoped_handle scope(new_fill_dir(L, function, buffer));
      luaX_push(L, offset);
      lua_pushvalue(L, file_info_index);
      if (lua_pcall(L, 5, 1, 0) == 0) {
        if (luaX_is_integer(L, -1)) {
          convert(L, file_info_index, file_info);
          return lua_tointeger(L, -1);
        }
        // return 0;
      }
      return -ENOSYS;
    }

    int open(const char* path, struct fuse_file_info* file_info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->get_field(L) == LUA_TNIL) {
        return -ENOSYS;
      }
      if (luaX_get_field(L, -1, "open") == LUA_TNIL) {
        return -ENOSYS;
      }
      lua_pushvalue(L, -2);
      luaX_push(L, path);
      convert(L, file_info);
      if (lua_pcall(L, 3, 1, 0) == 0) {
        if (luaX_is_integer(L, -1)) {
          return lua_tointeger(L, -1);
        }
      }
      return 0;
    }

    struct fuse_operations construct_operations() {
      struct fuse_operations operations;
      memset(&operations, 0, sizeof(operations));

      DROMOZOA_SET_OPERATION(init);
      DROMOZOA_SET_OPERATION(destroy);
      DROMOZOA_SET_OPERATION(getattr);
      DROMOZOA_SET_OPERATION(readdir);
      DROMOZOA_SET_OPERATION(open);
      DROMOZOA_SET_OPERATION(getxattr);

      return operations;
    }
  }

  struct fuse_operations operations = construct_operations();
}
