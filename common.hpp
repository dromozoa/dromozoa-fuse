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

#include <list>

#include <dromozoa/bind.hpp>
#include <dromozoa/bind/mutex.hpp>

namespace dromozoa {
  class operations {
  public:
    operations(lua_State*, int);
    fuse_operations* get();
    lua_State* state() const;
    bool prepare(lua_State*, const char*) const;
    dromozoa::mutex& mutex();
  private:
    fuse_operations ops_;
    luaX_reference<> ref_;
    dromozoa::mutex mutex_;
    operations(const operations&);
    operations& operator=(const operations&);
    bool check(lua_State*, const char*) const;
  };

  class state_pool {
  public:
    state_pool(size_t, size_t, const std::string&, const std::string&);
    ~state_pool();
    lua_State* open();
    void close(lua_State*);
  private:
    size_t max_idle_states_;
    std::string chunk_;
    std::string name_;
    mutex mutex_;
    size_t active_states_;
    std::list<lua_State*> idle_states_;
    state_pool(const state_pool&);
    state_pool& operator=(const state_pool&);
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
  bool convert(lua_State*, int, struct fuse_conn_info*);
  bool convert(lua_State*, int, struct fuse_file_info*);
  bool convert(lua_State*, int, struct flock*);
  bool convert(lua_State*, int, struct stat*);
  bool convert(lua_State*, int, struct statvfs*);
}

#endif
