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

#include <iostream>
#include <vector>
#include <string>

namespace dromozoa {
  namespace {
    void impl_main(lua_State* L) {
      luaL_checktype(L, 1, LUA_TTABLE);
      operations_handle* operations = check_operations_handle(L, 2);

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

      void* data = reinterpret_cast<void*>(0xFEEDFACE);
      std::cerr << "main " << data << "\n";
      int result = fuse_main(argv.size() - 1, argv.data(), operations->get(), data);
      luaX_push(L, result);
    }
  }

  void initialize_main(lua_State* L) {
    luaX_set_field(L, -1, "main", impl_main);
  }
}
