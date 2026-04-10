#include <gtest/gtest.h>
#include <filesystem>
#include "fsearch/index_store.h"
#include "fsearch/inverted_index.h"

using namespace fsearch;

TEST(IndexStoreTest, SaveLoadRoundTrip) {
  InvertedIndex a;
  auto d = a.add_document({"/hello.txt", 42, 1234567});
  a.add_term(d, "hello");
  a.add_term(d, "world");

  auto tmp = std::filesystem::temp_directory_path() / "fsi_test.bin";
  ASSERT_TRUE(IndexStore::save(a, tmp.string()));

  InvertedIndex b;
  ASSERT_TRUE(IndexStore::load(b, tmp.string()));
  ASSERT_EQ(b.num_docs(), 1u);
  EXPECT_EQ(b.document(0).path, "/hello.txt");
  EXPECT_EQ(b.postings("hello").size(), 1u);
  EXPECT_EQ(b.postings("world").size(), 1u);
  std::filesystem::remove(tmp);
}

TEST(IndexStoreTest, LoadRejectsBadMagic) {
  auto tmp = std::filesystem::temp_directory_path() / "fsi_bad.bin";
  {
    std::ofstream f(tmp, std::ios::binary);
    f << "XXXX" << '\0';
  }
  InvertedIndex b;
  EXPECT_FALSE(IndexStore::load(b, tmp.string()));
  std::filesystem::remove(tmp);
}
