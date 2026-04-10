#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "fsearch/walker.h"

namespace fs = std::filesystem;
using namespace fsearch;

namespace {
fs::path make_tmp_tree() {
  auto root = fs::temp_directory_path() / "fsearch_walker_test";
  fs::remove_all(root);
  fs::create_directories(root / "sub");
  std::ofstream(root / "a.txt") << "aaa";
  std::ofstream(root / "b.md") << "bbb";
  std::ofstream(root / "sub" / "c.txt") << "ccc";
  std::ofstream(root / "ignore.o") << "x";
  return root;
}
}  // namespace

TEST(WalkerTest, ListsRegularFilesAndRespectsExcludes) {
  auto root = make_tmp_tree();
  Walker w(root);
  int n = 0;
  std::vector<std::string> seen;
  w.walk([&](const FileEntry& fe) {
    ++n;
    seen.push_back(fe.path.filename().string());
  });
  EXPECT_EQ(n, 3);
  for (auto& s : seen) EXPECT_NE(s, "ignore.o");
  fs::remove_all(root);
}

TEST(WalkerTest, SkipsHiddenDirectories) {
  auto root = fs::temp_directory_path() / "fsearch_walker_hidden";
  fs::remove_all(root);
  fs::create_directories(root / ".hidden");
  std::ofstream(root / "keep.txt") << "k";
  std::ofstream(root / ".hidden" / "no.txt") << "n";
  Walker w(root);
  int n = 0;
  w.walk([&](const FileEntry&) { ++n; });
  EXPECT_EQ(n, 1);
  fs::remove_all(root);
}
