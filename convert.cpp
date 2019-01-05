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

#define DROMOZOA_SET_FIELD(name) \
  luaX_set_field(L, index, #name, that->name) \
  /**/

#define DROMOZOA_OPT_FIELD(name) \
  that->name = luaX_opt_integer_field(L, index, #name, that->name) \
  /**/

namespace dromozoa {
  int convert(lua_State* L, const struct fuse_context* that) {
    lua_newtable(L);
    int index = lua_gettop(L);
    DROMOZOA_SET_FIELD(uid);
    DROMOZOA_SET_FIELD(gid);
    DROMOZOA_SET_FIELD(pid);
    DROMOZOA_SET_FIELD(umask);
    return index;
  }

  int convert(lua_State* L, const struct fuse_conn_info* that) {
    lua_newtable(L);
    int index = lua_gettop(L);
    DROMOZOA_SET_FIELD(proto_major);
    DROMOZOA_SET_FIELD(proto_minor);
    DROMOZOA_SET_FIELD(async_read);
    DROMOZOA_SET_FIELD(max_write);
    DROMOZOA_SET_FIELD(max_readahead);
    DROMOZOA_SET_FIELD(capable);
    DROMOZOA_SET_FIELD(want);
    DROMOZOA_SET_FIELD(max_background);
    DROMOZOA_SET_FIELD(congestion_threshold);
    return index;
  }

  int convert(lua_State* L, const struct fuse_file_info* that) {
    lua_newtable(L);
    int index = lua_gettop(L);
    DROMOZOA_SET_FIELD(flags);
    DROMOZOA_SET_FIELD(writepage);
    DROMOZOA_SET_FIELD(direct_io);
    DROMOZOA_SET_FIELD(keep_cache);
    DROMOZOA_SET_FIELD(flush);
    DROMOZOA_SET_FIELD(nonseekable);
    DROMOZOA_SET_FIELD(flock_release);
    DROMOZOA_SET_FIELD(fh);
    DROMOZOA_SET_FIELD(lock_owner);
    return index;
  }

  bool convert(lua_State* L, int index, struct fuse_conn_info* that) {
    if (lua_istable(L, index)) {
      DROMOZOA_OPT_FIELD(proto_major); // read-only
      DROMOZOA_OPT_FIELD(proto_minor); // read-only
      DROMOZOA_OPT_FIELD(async_read);  // read-write
      DROMOZOA_OPT_FIELD(max_write);
      DROMOZOA_OPT_FIELD(max_readahead);
      DROMOZOA_OPT_FIELD(capable);
      DROMOZOA_OPT_FIELD(want);
      DROMOZOA_OPT_FIELD(max_background);
      DROMOZOA_OPT_FIELD(congestion_threshold);
      return true;
    } else {
      return false;
    }
  }

  bool convert(lua_State* L, int index, struct fuse_file_info* that) {
    if (lua_istable(L, index)) {
      DROMOZOA_OPT_FIELD(flags);
      DROMOZOA_OPT_FIELD(writepage);
      DROMOZOA_OPT_FIELD(direct_io);
      DROMOZOA_OPT_FIELD(keep_cache);
      DROMOZOA_OPT_FIELD(flush);
      DROMOZOA_OPT_FIELD(nonseekable);
      DROMOZOA_OPT_FIELD(flock_release);
      DROMOZOA_OPT_FIELD(fh);
      DROMOZOA_OPT_FIELD(lock_owner);
      return true;
    } else {
      return false;
    }
  }

  bool convert(lua_State* L, int index, struct stat* that) {
    if (lua_istable(L, index)) {
      memset(that, 0, sizeof(*that));
      DROMOZOA_OPT_FIELD(st_dev);
      DROMOZOA_OPT_FIELD(st_ino);
      DROMOZOA_OPT_FIELD(st_mode);
      DROMOZOA_OPT_FIELD(st_nlink);
      DROMOZOA_OPT_FIELD(st_uid);
      DROMOZOA_OPT_FIELD(st_gid);
      DROMOZOA_OPT_FIELD(st_size);
      DROMOZOA_OPT_FIELD(st_atime);
      DROMOZOA_OPT_FIELD(st_mtime);
      DROMOZOA_OPT_FIELD(st_ctime);
      DROMOZOA_OPT_FIELD(st_blksize);
      DROMOZOA_OPT_FIELD(st_blocks);
      return true;
    } else {
      return false;
    }
  }

  bool convert(lua_State* L, int index, struct statvfs* that) {
    if (lua_istable(L, index)) {
      memset(that, 0, sizeof(*that));
      DROMOZOA_OPT_FIELD(f_bsize);
      DROMOZOA_OPT_FIELD(f_frsize);
      DROMOZOA_OPT_FIELD(f_blocks);
      DROMOZOA_OPT_FIELD(f_bfree);
      DROMOZOA_OPT_FIELD(f_bavail);
      DROMOZOA_OPT_FIELD(f_files);
      DROMOZOA_OPT_FIELD(f_ffree);
      DROMOZOA_OPT_FIELD(f_fsid);
      DROMOZOA_OPT_FIELD(f_flag);
      DROMOZOA_OPT_FIELD(f_namemax);
      return true;
    } else {
      return false;
    }
  }
}
