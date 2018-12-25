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

#include <string.h>

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
      luaX_reference<>* self = static_cast<luaX_reference<>*>(userdata);
      self->~luaX_reference();
    }

    struct fuse_operations construct_operations() {
      struct fuse_operations operations;
      memset(&operations, sizeof(operations), 0);

      operations.init = init;
      operations.destroy = destroy;

      return operations;
    }
  }

  struct fuse_operations operations = construct_operations();
}
