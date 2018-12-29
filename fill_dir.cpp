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

namespace dromozoa {
  namespace {
    class fill_dir {
      explicit fill_dir(fuse_fill_dir_t function, void* buffer)
        : function_(function),
          buffer_(buffer) {}

      int operator()(const char* name, const struct stat* buffer, off_t offset) {
        return function_(buffer_, name, buffer, offset);
      }

    public:
      fuse_fill_dir_t function_;
      void* buffer_;
      fill_dir(const fill_dir&);
      fill_dir& operator=(const fill_dir&);
    };
  }

  void initialize_fill_dir(lua_State* L) {
  }
}
