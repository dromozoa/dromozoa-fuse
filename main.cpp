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

#include <vector>
#include <string>

namespace dromozoa {
  extern struct fuse_operations operations;

  namespace {
    void impl_main(lua_State* L) {
      luaL_checktype(L, 1, LUA_TTABLE);
      luaL_checktype(L, 2, LUA_TTABLE);

      std::vector<std::string> args;
      for (int i = 1; ; ++i) {
        luaX_get_field(L, 1, i);
        if (const char* p = lua_tostring(L, -1)) {
          args.push_back(p);
          lua_pop(L, 1);
        } else {
          lua_pop(L, 1);
          break;
        }
      }

      std::vector<std::string>::const_iterator i = args.begin();
      std::vector<std::string>::const_iterator end = args.end();

      std::vector<char*> argv;
      for (; i != end; ++i) {
        argv.push_back(const_cast<char*>(i->c_str()));
      }
      argv.push_back(0);

      scoped_ptr<luaX_reference<> > self(new luaX_reference<>(L, 2));
      int result = fuse_main(argv.size() - 1, argv.data(), &operations, self.release());
      luaX_push(L, result);
    }

    void impl_get_context(lua_State* L) {
      convert(L, fuse_get_context());
    }
  }

  void initialize_main(lua_State* L) {
    luaX_set_field(L, -1, "main", impl_main);
    luaX_set_field(L, -1, "get_context", impl_get_context);

    luaX_set_field(L, -1, "FUSE_CAP_ASYNC_READ", FUSE_CAP_ASYNC_READ);
    luaX_set_field(L, -1, "FUSE_CAP_POSIX_LOCKS", FUSE_CAP_POSIX_LOCKS);
    luaX_set_field(L, -1, "FUSE_CAP_ATOMIC_O_TRUNC", FUSE_CAP_ATOMIC_O_TRUNC);
    luaX_set_field(L, -1, "FUSE_CAP_EXPORT_SUPPORT", FUSE_CAP_EXPORT_SUPPORT);
    luaX_set_field(L, -1, "FUSE_CAP_BIG_WRITES", FUSE_CAP_BIG_WRITES);
    luaX_set_field(L, -1, "FUSE_CAP_DONT_MASK", FUSE_CAP_DONT_MASK);
    luaX_set_field(L, -1, "FUSE_CAP_SPLICE_WRITE", FUSE_CAP_SPLICE_WRITE);
    luaX_set_field(L, -1, "FUSE_CAP_SPLICE_MOVE", FUSE_CAP_SPLICE_MOVE);
    luaX_set_field(L, -1, "FUSE_CAP_SPLICE_READ", FUSE_CAP_SPLICE_READ);
    luaX_set_field(L, -1, "FUSE_CAP_FLOCK_LOCKS", FUSE_CAP_FLOCK_LOCKS);
    luaX_set_field(L, -1, "FUSE_CAP_IOCTL_DIR", FUSE_CAP_IOCTL_DIR);

    luaX_set_field(L, -1, "F_OK", F_OK);
    luaX_set_field(L, -1, "R_OK", R_OK);
    luaX_set_field(L, -1, "W_OK", W_OK);
    luaX_set_field(L, -1, "X_OK", X_OK);
  }
}
