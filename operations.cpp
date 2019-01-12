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
  do { \
    if (check(L, #name)) { \
      ops_.name = name; \
    } \
  } while (false) \
  /**/

namespace dromozoa {
  namespace {
    template <class T>
    class scoped_converter {
    public:
      scoped_converter(lua_State* state, T* ptr)
        : state_(state),
          ptr_(ptr),
          index_(convert(state, ptr)) {}

      ~scoped_converter() {
        convert(state_, index_, ptr_);
      }

      int index() const {
        return index_;
      }

    private:
      lua_State* state_;
      T* ptr_;
      int index_;
      scoped_converter(const scoped_converter&);
      scoped_converter& operator=(const scoped_converter&);
    };

    typedef scoped_converter<struct fuse_conn_info> conn_info;
    typedef scoped_converter<struct fuse_file_info> file_info;

    int call(lua_State* L, int nargs, int d = 0) {
      if (lua_pcall(L, nargs, 1, 0) == 0) {
        if (luaX_is_integer(L, -1)) {
          return lua_tointeger(L, -1);
        } else if (lua_isnil(L, -1)) {
          return d;
        }
        DROMOZOA_UNEXPECTED("must return an integer");
      } else {
        if (luaX_is_integer(L, -1)) {
          return lua_tointeger(L, -1);
        }
        DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/stat.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L89
    int getattr(const char* path, struct stat* buffer) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "getattr")) {
        luaX_push(L, path);
        if (lua_pcall(L, 2, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          } else if (convert(L, -1, buffer)) {
            return 0;
          }
          DROMOZOA_UNEXPECTED("must return a table");
        } else {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/readlink.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L97
    int readlink(const char* path, char* buffer, size_t size) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "readlink")) {
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
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/mknod.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L110
    int mknod(const char* path, mode_t mode, dev_t dev) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "mknod")) {
        luaX_push(L, path, mode, dev);
        return call(L, 4);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/mkdir.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L118
    int mkdir(const char* path, mode_t mode) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "mkdir")) {
        luaX_push(L, path, mode);
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/unlink.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L126
    int unlink(const char* path) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "unlink")) {
        luaX_push(L, path);
        return call(L, 2);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/rmdir.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L129
    int rmdir(const char* path) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "rmdir")) {
        luaX_push(L, path);
        return call(L, 2);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/symlink.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L132
    int symlink(const char* target, const char* path) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "symlink")) {
        luaX_push(L, target, path);
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/rename.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L135
    int rename(const char* oldpath, const char* newpath) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "rename")) {
        luaX_push(L, oldpath, newpath);
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/link.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L138
    int link(const char* oldpath, const char* newpath) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "link")) {
        luaX_push(L, oldpath, newpath);
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/chmod.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L142
    int chmod(const char* path, mode_t mode) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "chmod")) {
        luaX_push(L, path, mode);
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/chown.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L144
    int chown(const char* path, uid_t uid, gid_t gid) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "chown")) {
        luaX_push(L, path, uid, gid);
        return call(L, 4);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/truncate.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L147
    int truncate(const char* path, off_t size) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "truncate")) {
        luaX_push(L, path, size);
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/open.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L156
    int open(const char* path, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "open")) {
        luaX_push(L, path);
        lua_pushvalue(L, info.index());
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/read.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L175
    int read(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "read")) {
        luaX_push(L, path, size, offset);
        lua_pushvalue(L, info.index());
        if (lua_pcall(L, 5, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          } else if (luaX_string_reference result = luaX_to_string(L, -1)) {
            memset(buffer, 0, size);
            memcpy(buffer, result.data(), std::min(size, result.size()));
            return result.size();
          }
          DROMOZOA_UNEXPECTED("must return a string");
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/write.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L189
    int write(const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "write")) {
        luaX_push(L, path, luaX_string_reference(buffer, size), offset);
        lua_pushvalue(L, info.index());
        return call(L, 5, size);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/statvfs.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L200
    int statfs(const char* path, struct statvfs* buffer) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "statfs")) {
        luaX_push(L, path);
        if (lua_pcall(L, 2, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          } else if (convert(L, -1, buffer)) {
            return 0;
          }
          DROMOZOA_UNEXPECTED("must return a table");
        } else {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L209
    int flush(const char* path, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "flush")) {
        luaX_push(L, path);
        lua_pushvalue(L, info.index());
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L234
    int release(const char* path, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "release")) {
        luaX_push(L, path);
        lua_pushvalue(L, info.index());
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/fsync.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L250
    int fsync(const char* path, int datasync, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "fsync")) {
        luaX_push(L, path, datasync);
        lua_pushvalue(L, info.index());
        return call(L, 4);
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L271
    int opendir(const char* path, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "opendir")) {
        luaX_push(L, path);
        lua_pushvalue(L, info.index());
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L283
    int readdir(const char* path, void* buffer, fuse_fill_dir_t function, off_t offset, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "readdir")) {
        luaX_push(L, path);
        scoped_handle scope(new_fill_dir(L, function, buffer));
        luaX_push(L, offset);
        lua_pushvalue(L, info.index());
        return call(L, 5);
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L309
    int releasedir(const char* path, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "releasedir")) {
        luaX_push(L, path);
        lua_pushvalue(L, info.index());
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L313
    int fsyncdir(const char* path, int datasync, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "fsyncdir")) {
        luaX_push(L, path, datasync);
        lua_pushvalue(L, info.index());
        return call(L, 4);
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L322
    void* init(struct fuse_conn_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      conn_info info(L, info_ptr);
      if (self->prepare(L, "init")) {
        lua_pushvalue(L, info.index());
        if (lua_pcall(L, 2, 0, 0) != 0) {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return self;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L334
    void destroy(void* userdata) {
      scoped_ptr<operations> self(static_cast<operations*>(userdata));
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "destroy")) {
        if (lua_pcall(L, 1, 0, 0) != 0) {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/access.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L343
    int access(const char* path, int mode) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "access")) {
        luaX_push(L, path, mode);
        return call(L, 3);
      }
      return -ENOSYS;
    }

    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L356
    int create(const char* path, mode_t mode, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "create")) {
        luaX_push(L, path, mode);
        lua_pushvalue(L, info.index());
        return call(L, 4);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/ftruncate.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L370
    int ftruncate(const char* path, off_t size, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "ftruncate")) {
        luaX_push(L, path, size);
        lua_pushvalue(L, info.index());
        return call(L, 4);
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/fstat.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L384
    int fgetattr(const char* path, struct stat* buffer, struct fuse_file_info* info_ptr) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      file_info info(L, info_ptr);
      if (self->prepare(L, "fgetattr")) {
        luaX_push(L, path);
        lua_pushvalue(L, info.index());
        if (lua_pcall(L, 3, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          } else if (convert(L, -1, buffer)) {
            return 0;
          }
          DROMOZOA_UNEXPECTED("must return a table");
        } else {
          if (luaX_is_integer(L, -1)) {
            return lua_tointeger(L, -1);
          }
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
        }
      }
      return -ENOSYS;
    }

    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/utimensat.2.html
    // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L433
    int utimens(const char* path, const struct timespec times[2]) {
      operations* self = static_cast<operations*>(fuse_get_context()->private_data);
      lua_State* L = self->state();
      luaX_top_saver save(L);
      if (self->prepare(L, "utimens")) {
        luaX_push(L, path);
        if (times) {
          convert(L, &times[0]);
          convert(L, &times[1]);
        } else {
          luaX_push(L, luaX_nil, luaX_nil);
        }
        return call(L, 4);
      }
      return -ENOSYS;
    }
  }

  operations::operations(lua_State* L, int index)
    : ops_(),
      ref_(L, index) {
    ops_.init = init;
    ops_.destroy = destroy;

    DROMOZOA_SET_OPERATION(getattr);
    DROMOZOA_SET_OPERATION(readlink);
    DROMOZOA_SET_OPERATION(mknod);
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
    DROMOZOA_SET_OPERATION(statfs);
    DROMOZOA_SET_OPERATION(flush);
    DROMOZOA_SET_OPERATION(release);
    DROMOZOA_SET_OPERATION(fsync);
    DROMOZOA_SET_OPERATION(opendir);
    DROMOZOA_SET_OPERATION(readdir);
    DROMOZOA_SET_OPERATION(releasedir);
    DROMOZOA_SET_OPERATION(fsyncdir);
    DROMOZOA_SET_OPERATION(access);
    DROMOZOA_SET_OPERATION(create);
    DROMOZOA_SET_OPERATION(ftruncate);
    DROMOZOA_SET_OPERATION(fgetattr);
    DROMOZOA_SET_OPERATION(utimens);
  }

  fuse_operations* operations::get() {
    return &ops_;
  }

  lua_State* operations::state() const {
    return ref_.state();
  }

  bool operations::prepare(lua_State* L, const char* name) const {
    if (ref_.get_field(L) != LUA_TNIL && luaX_get_field(L, -1, name) != LUA_TNIL) {
      lua_pushvalue(L, -2);
      return true;
    } else {
      return false;
    }
  }

  bool operations::check(lua_State* L, const char* name) const {
    luaX_top_saver save(L);
    return ref_.get_field(L) != LUA_TNIL && luaX_get_field(L, -1, name) != LUA_TNIL;
  }
}
