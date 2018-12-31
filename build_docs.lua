-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-fuse.
--
-- dromozoa-fuse is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-fuse is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-fuse.  If not, see <http://www.gnu.org/licenses/>.

local header_prolog = [[
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
<title>%s</title>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/github-markdown-css/2.10.0/github-markdown.min.css">
<style>
.markdown-body {
  box-sizing: border-box;
  min-width: 200px;
  max-width: 980px;
  margin: 0 auto;
  padding: 45px;
}
@media (max-width: 767px) {
  .markdown-body {
    padding: 15px;
  }
}
pre.prettyprint > ol.linenums {
  padding-left: 4em;
}
pre.prettyprint > ol.linenums > li {
  color: #C6C6C6;
  list-style-type: decimal;
}
</style>
</head>
<body>
<div class="markdown-body">

<h1>%s</h1>

<pre class="prettyprint lang-c linenums">
]]

local header_epilog = [[
</pre>
</div>
<script>
(function (root) {
  if (!root.exports) {
    root.exports = {};
  }
  root.exports["dromozoa-fuse"] = function () {
    let document = root.document;
    let items = document.querySelectorAll("pre.prettyprint > ol.linenums > li");
    for (let i = 0; i < items.length; ++i) {
      items[i].setAttribute("id", "L" + (i + 1));
    }
    if (document.location.hash) {
      document.location.href = document.location.hash;
    }
  };
}(this));
</script>
<script src="https://cdn.rawgit.com/google/code-prettify/master/loader/run_prettify.js?callback=dromozoa-fuse"></script>
</body>
</html>
]]

local escape_table = { ["&"] = "&amp;", ["<"] = "&lt;", [">"] = "&gt;" }
local function escape(source)
  return (source:gsub("[&<>]", escape_table))
end

local function process_header(source_filename, result_filename)
  local handle = assert(io.open(source_filename))
  local out = assert(io.open(result_filename, "w"))

  out:write(header_prolog:format(source_filename, source_filename))
  for line in handle:lines() do
    out:write(escape(line), "\n")
  end
  out:write(header_epilog)

  handle:close()
  out:close()
end

local dir = "docs/fuse-2.9.2"
process_header(dir .. "/fuse.h", dir .. "/fuse.h.html")
process_header(dir .. "/fuse_common.h", dir .. "/fuse_common.h.html")
