// Copyright (C) 2018-2020 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 28

#ifdef HAVE_OSXFUSE_FUSE_H
#include <osxfuse/fuse.h>
#else
#include <fuse.h>
#endif

#if FUSE_VERSION < 28
#error libfuse 2.8 or newer required
#endif

#include <dromozoa/bind.hpp>

namespace dromozoa {
  class state_manager {
  public:
    virtual ~state_manager() = 0;
    virtual lua_State* open() = 0;
    virtual void close(lua_State*) = 0;
  };

  state_manager* check_state_manager(lua_State*, int);

  class managed_state {
  public:
    explicit managed_state(state_manager*);
    ~managed_state();
    lua_State* get() const;
  private:
    state_manager* manager_;
    lua_State* state_;
    managed_state(const managed_state&);
    managed_state& operator=(const managed_state&);
  };

  class xattr_cache_item {
  public:
    xattr_cache_item(uint64_t time, const char* path, const char* data, size_t size)
      : time_(time),
        path_(path),
        data_(data, size) {}

    uint64_t time() const {
      return time_;
    }

    const std::string& data() const {
      return data_;
    }

  private:
    uint64_t time_;
    std::string path_;
    std::string data_;
  };

  class xattr_cache {
  public:

  private:
    std::map<std::string, std::string> map_;
    std::list<std::string> list_;
  };

  class operations {
  public:
    operations(state_manager*);
    fuse_operations* get();
    state_manager* manager() const;
  private:
    fuse_operations ops_;
    state_manager* manager_;
    operations(const operations&);
    operations& operator=(const operations&);
  };

  class handle {
  public:
    virtual ~handle() = 0;
    virtual void reset() = 0;
  };

  class scoped_handle {
  public:
    explicit scoped_handle(handle*);
    ~scoped_handle();
  private:
    handle* ptr_;
    scoped_handle(const scoped_handle&);
    scoped_handle& operator=(const scoped_handle&);
  };

  handle* new_fill_dir(lua_State*, fuse_fill_dir_t, void*);

  int convert(lua_State*, const struct fuse_context*);
  int convert(lua_State*, const struct fuse_conn_info*);
  int convert(lua_State*, const struct fuse_file_info*);
  int convert(lua_State*, const struct flock*);
  int convert(lua_State*, const struct timespec*);
  bool convert(lua_State*, int, struct fuse_operations*);
  bool convert(lua_State*, int, struct fuse_conn_info*);
  bool convert(lua_State*, int, struct fuse_file_info*);
  bool convert(lua_State*, int, struct flock*);
  bool convert(lua_State*, int, struct stat*);
  bool convert(lua_State*, int, struct statvfs*);
}

#endif
