#pragma once
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace fsearch {

struct FileEntry {
  std::filesystem::path path;
  std::uintmax_t size = 0;
  std::filesystem::file_time_type mtime{};
};

// A simple non-recursive-stack-based walker. Calls `visitor` for each
// regular file found under `root`.
class Walker {
 public:
  using Visitor = std::function<void(const FileEntry&)>;

  explicit Walker(std::filesystem::path root) : root_(std::move(root)) {}

  void set_skip_hidden(bool s) { skip_hidden_ = s; }
  void add_exclude(std::string ext) { excluded_exts_.push_back(std::move(ext)); }

  void walk(const Visitor& v) const;

 private:
  std::filesystem::path root_;
  bool skip_hidden_ = true;
  std::vector<std::string> excluded_exts_{".o", ".obj", ".exe", ".bin"};
};

}  // namespace fsearch
