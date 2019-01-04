// Copyright (C) 2018,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include <algorithm>

#define DROMOZOA_SET_OPERATION(name) \
  operations.name = name \
  /**/

namespace dromozoa {
  namespace {
    bool check(luaX_reference<>* self, lua_State* L, const char* name) {
      if (self->get_field(L) == LUA_TNIL || luaX_get_field(L, -1, name) == LUA_TNIL) {
        return false;
      } else {
        lua_pushvalue(L, -2);
        return true;
      }
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/stat.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L89
    int getattr(const char* path, struct stat* buffer) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "getattr")) {
        luaX_push(L, path);
        if (lua_pcall(L, 2, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          } else if (convert(L, -1, buffer)) {
            return 0;
          }
          DROMOZOA_UNEXPECTED("must return a table");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/readlink.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L97
    int readlink(const char* path, char* buffer, size_t size) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "readlink")) {
        luaX_push(L, path, size);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          } else if (luaX_string_reference result = luaX_to_string(L, -1)) {
            memset(buffer, 0, size);
            memcpy(buffer, result.data(), std::min(size - 1, result.size()));
            return 0;
          }
          DROMOZOA_UNEXPECTED("must return a string");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/mkdir.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L118
    int mkdir(const char* path, mode_t mode) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "mkdir")) {
        luaX_push(L, path, mode);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/unlink.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L126
    int unlink(const char* path) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "unlink")) {
        luaX_push(L, path);
        if (lua_pcall(L, 2, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/rmdir.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L129
    int rmdir(const char* path) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "rmdir")) {
        luaX_push(L, path);
        if (lua_pcall(L, 2, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/symlink.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L132
    int symlink(const char* source, const char* result) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "symlink")) {
        luaX_push(L, source, result);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/rename.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L135
    int rename(const char* source, const char* result) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "rename")) {
        luaX_push(L, source, result);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/link.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L138
    int link(const char* source, const char* result) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "link")) {
        luaX_push(L, source, result);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/chmod.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L142
    int chmod(const char* path, mode_t mode) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "chmod")) {
        luaX_push(L, path, mode);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/chown.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L144
    int chown(const char* path, uid_t uid, gid_t gid) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "chown")) {
        luaX_push(L, path, uid, gid);
        if (lua_pcall(L, 4, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/truncate.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L147
    int truncate(const char* path, off_t size) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "truncate")) {
        luaX_push(L, path, size);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/open.2.html
    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L156
    int open(const char* path, struct fuse_file_info* info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      int info_index = convert(L, info);
      if (check(self, L, "open")) {
        luaX_push(L, path);
        lua_pushvalue(L, info_index);
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            convert(L, info_index, info);
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/read.2.html
    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L175
    int read(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      int info_index = convert(L, info);
      if (check(self, L, "read")) {
        luaX_push(L, path, size, offset);
        lua_pushvalue(L, info_index);
        if (lua_pcall(L, 5, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            convert(L, info_index, info);
            return lua_tointeger(L, -1);
          } else if (luaX_string_reference result = luaX_to_string(L, -1)) {
            memset(buffer, 0, size);
            memcpy(buffer, result.data(), std::min(size, result.size()));
            convert(L, info_index, info);
            return result.size();
          }
          DROMOZOA_UNEXPECTED("must return a string");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/write.2.html
    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L189
    int write(const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      int info_index = convert(L, info);
      if (check(self, L, "write")) {
        luaX_push(L, path, luaX_string_reference(buffer, size), offset);
        lua_pushvalue(L, info_index);
        if (lua_pcall(L, 5, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            convert(L, info_index, info);
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/getxattr.2.html
    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L262
    int getxattr(const char* path, const char* name, char* value, size_t size) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self, L, "getxattr")) {
        luaX_push(L, path, name, size);
        if (lua_pcall(L, 4, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          } else if (luaX_string_reference result = luaX_to_string(L, -1)) {
            if (result.size() <= size) {
              memset(value, 0, size);
              memcpy(value, result.data(), result.size());
              return result.size();
            } else {
              return -ERANGE;
            }
          }
          DROMOZOA_UNEXPECTED("must return a string");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/readdir.2.html
    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L283
    int readdir(const char* path, void* buffer, fuse_fill_dir_t function, off_t offset, struct fuse_file_info* info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      int info_index = convert(L, info);
      if (check(self, L, "readdir")) {
        luaX_push(L, path);
        scoped_handle scope(new_fill_dir(L, function, buffer));
        luaX_push(L, offset);
        lua_pushvalue(L, info_index);
        if (lua_pcall(L, 5, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            convert(L, info_index, info);
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L322
    void* init(struct fuse_conn_info* info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      int info_index = convert(L, info);
      if (check(self, L, "init")) {
        lua_pushvalue(L, info_index);
        if (lua_pcall(L, 2, 0, 0) == 0) {
          convert(L, info_index, info);
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return self;
    }

    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L334
    void destroy(void* userdata) {
      scoped_ptr<luaX_reference<> > self(static_cast<luaX_reference<>*>(userdata));
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (check(self.get(), L, "destroy")) {
        if (lua_pcall(L, 1, 0, 0) != 0) {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
    }

    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/mknod.2.html
    // http://linuxjm.osdn.jp/html/LDP_man-pages/man2/open.2.html
    // http://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L356
    int create(const char* path, mode_t mode, struct fuse_file_info* info) {
      luaX_reference<>* self = static_cast<luaX_reference<>*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      int info_index = convert(L, info);
      if (check(self, L, "create")) {
        luaX_push(L, path, mode);
        lua_pushvalue(L, info_index);
        if (lua_pcall(L, 4, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            convert(L, info_index, info);
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED("must return an integer");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    struct fuse_operations construct_operations() {
      struct fuse_operations operations;
      memset(&operations, 0, sizeof(operations));

      DROMOZOA_SET_OPERATION(getattr);
      DROMOZOA_SET_OPERATION(readlink);
      DROMOZOA_SET_OPERATION(mkdir);
      DROMOZOA_SET_OPERATION(unlink);
      DROMOZOA_SET_OPERATION(rmdir);
      DROMOZOA_SET_OPERATION(symlink);
      DROMOZOA_SET_OPERATION(rename);
      DROMOZOA_SET_OPERATION(link);
      DROMOZOA_SET_OPERATION(chmod);
      DROMOZOA_SET_OPERATION(chown);
      DROMOZOA_SET_OPERATION(truncate);
      DROMOZOA_SET_OPERATION(open);
      DROMOZOA_SET_OPERATION(read);
      DROMOZOA_SET_OPERATION(write);
      DROMOZOA_SET_OPERATION(getxattr);
      DROMOZOA_SET_OPERATION(readdir);
      DROMOZOA_SET_OPERATION(init);
      DROMOZOA_SET_OPERATION(destroy);
      DROMOZOA_SET_OPERATION(create);

      return operations;
    }
  }

  struct fuse_operations operations = construct_operations();
}
