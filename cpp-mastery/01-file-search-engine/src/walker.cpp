#include "fsearch/walker.h"

#include <algorithm>
#include <system_error>

namespace fsearch {

namespace fs = std::filesystem;

static bool is_hidden(const fs::path& p) {
  auto name = p.filename().string();
  return !name.empty() && name[0] == '.';
}

void Walker::walk(const Visitor& v) const {
  std::error_code ec;
  if (!fs::exists(root_, ec)) return;
  auto opts = fs::directory_options::skip_permission_denied;
  for (auto it = fs::recursive_directory_iterator(root_, opts, ec);
       it != fs::recursive_directory_iterator(); it.increment(ec)) {
    if (ec) { ec.clear(); continue; }
    const auto& entry = *it;
    const auto& p = entry.path();
    if (skip_hidden_ && is_hidden(p)) {
      if (entry.is_directory()) it.disable_recursion_pending();
      continue;
    }
    if (!entry.is_regular_file(ec)) { ec.clear(); continue; }
    auto ext = p.extension().string();
    if (std::find(excluded_exts_.begin(), excluded_exts_.end(), ext) != excluded_exts_.end())
      continue;

    FileEntry fe;
    fe.path  = p;
    fe.size  = entry.file_size(ec);
    fe.mtime = entry.last_write_time(ec);
    if (ec) { ec.clear(); continue; }
    v(fe);
  }
}

}  // namespace fsearch
