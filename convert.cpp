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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common.hpp"

#define DROMOZOA_SET_FIELD(name) \
  luaX_set_field(L, index, #name, that->name) \
  /**/

#define DROMOZOA_OPT_FIELD(name) \
  that->name = luaX_opt_integer_field(L, index, #name, that->name) \
  /**/

namespace dromozoa {
  namespace {
    bool convert_timespec(lua_State* L, int index, const char* key, struct timespec& tv) {
      int type = luaX_get_field(L, index, key);
      if (type == LUA_TNUMBER) {
        double t = lua_tonumber(L, -1);
        double i = 0;
        double f = modf(t, &i);
        tv.tv_sec = i;
        tv.tv_nsec = f * 1000000000;
        lua_pop(L, 1);
        return true;
      } else if (type == LUA_TTABLE) {
        tv.tv_sec = luaX_opt_integer_field<time_t>(L, -1, "tv_sec", 0);
        tv.tv_nsec = luaX_opt_integer_field<long>(L, -1, "tv_nsec", 0, 0L, 999999999L);
        lua_pop(L, 1);
        return true;
      } else {
        lua_pop(L, 1);
        return false;
      }
    }

    struct timespec convert_timespec(lua_State* L, int index, const char* key1, const char* key2, const char* key3) {
      struct timespec tv = {};
      if (convert_timespec(L, index, key1, tv)) {
        return tv;
      } else if (convert_timespec(L, index, key2, tv)) {
        return tv;
      } else if (convert_timespec(L, index, key3, tv)) {
        return tv;
      } else {
        return tv;
      }
    }
  }

  // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L593
  int convert(lua_State* L, const struct fuse_context* that) {
    lua_newtable(L);
    int index = lua_gettop(L);
    DROMOZOA_SET_FIELD(uid);
    DROMOZOA_SET_FIELD(gid);
    DROMOZOA_SET_FIELD(pid);
    DROMOZOA_SET_FIELD(umask);
    return index;
  }

  // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse_common.h.html#L133
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

  // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse_common.h.html#L40
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

  // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse_common.h.html#L133
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

  // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse_common.h.html#L40
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

  // https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_stat.h.html
  // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/stat.2.html
  // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L89
  bool convert(lua_State* L, int index, struct stat* that) {
    if (lua_istable(L, index)) {
      memset(that, 0, sizeof(*that));
      DROMOZOA_OPT_FIELD(st_dev); // ignored
      DROMOZOA_OPT_FIELD(st_ino); // ignored except if the 'use_ino' mount opetion is given
      DROMOZOA_OPT_FIELD(st_mode);
      DROMOZOA_OPT_FIELD(st_nlink);
      DROMOZOA_OPT_FIELD(st_uid);
      DROMOZOA_OPT_FIELD(st_gid);
      DROMOZOA_OPT_FIELD(st_size);

#if defined(HAVE_STRUCT_STAT_ST_ATIM)
      that->st_atim = convert_timespec(L, index, "st_atim", "st_atimespec", "st_atime");
#elif defined(HAVE_STRUCT_STAT_ST_ATIMESPEC)
      that->st_atimespec = convert_timespec(L, index, "st_atim", "st_atimespec", "st_atime");
#else
      that->st_atime = convert_timespec(L, index, "st_atim", "st_atimespec", "st_atime").tv_sec;
#endif

#if defined(HAVE_STRUCT_STAT_ST_MTIM)
      that->st_mtim = convert_timespec(L, index, "st_mtim", "st_mtimespec", "st_mtime");
#elif defined(HAVE_STRUCT_STAT_ST_MTIMESPEC)
      that->st_mtimespec = convert_timespec(L, index, "st_mtim", "st_mtimespec", "st_mtime");
#else
      that->st_mtime = convert_timespec(L, index, "st_mtim", "st_mtimespec", "st_mtime").tv_sec;
#endif

#if defined(HAVE_STRUCT_STAT_ST_CTIM)
      that->st_ctim = convert_timespec(L, index, "st_ctim", "st_ctimespec", "st_ctime");
#elif defined(HAVE_STRUCT_STAT_ST_CTIMESPEC)
      that->st_ctimespec = convert_timespec(L, index, "st_ctim", "st_ctimespec", "st_ctime");
#else
      that->st_ctime = convert_timespec(L, index, "st_ctim", "st_ctimespec", "st_ctime").tv_sec;
#endif

      DROMOZOA_OPT_FIELD(st_blksize); // ignored
      DROMOZOA_OPT_FIELD(st_blocks);
      return true;
    } else {
      return false;
    }
  }

  // https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_statvfs.h.html
  // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/statvfs.2.html
  // https://dromozoa.github.io/dromozoa-fuse/fuse-2.9.2/fuse.h.html#L200
  bool convert(lua_State* L, int index, struct statvfs* that) {
    if (lua_istable(L, index)) {
      memset(that, 0, sizeof(*that));
      DROMOZOA_OPT_FIELD(f_bsize);
      DROMOZOA_OPT_FIELD(f_frsize); // ignored
      DROMOZOA_OPT_FIELD(f_blocks);
      DROMOZOA_OPT_FIELD(f_bfree);
      DROMOZOA_OPT_FIELD(f_bavail);
      DROMOZOA_OPT_FIELD(f_files);
      DROMOZOA_OPT_FIELD(f_ffree);
      DROMOZOA_OPT_FIELD(f_favail); // ignored
      DROMOZOA_OPT_FIELD(f_fsid);   // ignored
      DROMOZOA_OPT_FIELD(f_flag);   // ignored
      DROMOZOA_OPT_FIELD(f_namemax);
      return true;
    } else {
      return false;
    }
  }
}
