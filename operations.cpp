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

namespace dromozoa {
  namespace {
    void* init(struct fuse_conn_info*) {
      return fuse_get_context()->private_data;
    }

    void destroy(void* userdata) {
      scoped_ptr<luaX_reference<> > self(static_cast<luaX_reference<>*>(userdata));
    }

    template <class T>
    void set_integer_field(lua_State* L, int index, const char* name, T& target) {
      target = luaX_opt_integer_field<T>(L, index, name, 0);
    }

    // to_tuple
    // to_table

    #define DROMOZOA_SET_INTEGER_FIELD(name) \
      set_integer_field(L, index, #name, target->name) \
      /**/

    void convert(lua_State* L, int index, struct stat* target) {
      memset(target, 0, sizeof(*target));
      DROMOZOA_SET_INTEGER_FIELD(st_dev);
      DROMOZOA_SET_INTEGER_FIELD(st_ino);
      DROMOZOA_SET_INTEGER_FIELD(st_mode);
      DROMOZOA_SET_INTEGER_FIELD(st_nlink);
      DROMOZOA_SET_INTEGER_FIELD(st_uid);
      DROMOZOA_SET_INTEGER_FIELD(st_gid);
      DROMOZOA_SET_INTEGER_FIELD(st_size);
      DROMOZOA_SET_INTEGER_FIELD(st_atime);
      DROMOZOA_SET_INTEGER_FIELD(st_mtime);
      DROMOZOA_SET_INTEGER_FIELD(st_ctime);
      DROMOZOA_SET_INTEGER_FIELD(st_blksize);
      DROMOZOA_SET_INTEGER_FIELD(st_blocks);
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

    // int readdir(const char* path, void* buffer, fuse_fill_dir_t fill, off_t offset, struct fuse_file_info* fi) {
    //   if (path) {
    //     std::cout << path << "\n";
    //   }
    //   return -ENOSYS;
    // }

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
      file_info_handle* handle = new_file_info_handle(L, file_info);
      if (lua_pcall(L, 3, 1, 0) == 0) {
        if (luaX_is_integer(L, -1)) {
          return lua_tointeger(L, -1);
        }
      }
      handle->reset();
      return 0;
    }

    struct fuse_operations construct_operations() {
      struct fuse_operations operations;
      memset(&operations, 0, sizeof(operations));

      operations.init = init;
      operations.destroy = destroy;
      operations.getattr = getattr;
      // operations.readdir = readdir;
      operations.open = open;
      operations.getxattr = getxattr;

      return operations;
    }
  }

  struct fuse_operations operations = construct_operations();
}
