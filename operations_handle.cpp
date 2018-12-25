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
#include <iostream>

// int (*getattr) (const char *, struct stat *);
// int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
// int (*open) (const char *, struct fuse_file_info *);
// int (*read) (const char *, char *, size_t, off_t, struct fuse_file_info *);

namespace dromozoa {
  namespace {
    void* init(struct fuse_conn_info*) {
      fuse_context* context = fuse_get_context();
      return context->private_data;
    }

    void destroy(void* userdata) {
      operations_handle* ops = static_cast<operations_handle*>(userdata);
      ops->unref();
    }

    int readdir(const char* path, void* buffer, fuse_fill_dir_t fill_dir, off_t offset, struct fuse_file_info* info) {
      return 0;
    }

    int getattr(const char* path, struct stat* stat) {
      return ENOENT;
    }
  }

  operations_handle::operations_handle()
    : operations_() {
    operations_.init = init;
    operations_.destroy = destroy;
    operations_.getattr = getattr;
  }

  void operations_handle::ref(lua_State* L, int index) {
    luaX_reference<>(L, index).swap(ref_);
  }

  void operations_handle::unref() {
    luaX_reference<>().swap(ref_);
  }

  struct fuse_operations* operations_handle::get() {
    return &operations_;
  }
}
