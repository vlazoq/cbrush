#include <gtest/gtest.h>
#include "fsearch/inverted_index.h"

using namespace fsearch;

TEST(InvertedIndexTest, DedupesPostingsPerDoc) {
  InvertedIndex idx;
  auto d = idx.add_document({"/a.txt", 10, 0});
  idx.add_term(d, "foo");
  idx.add_term(d, "foo");
  idx.add_term(d, "bar");
  EXPECT_EQ(idx.postings("foo").size(), 1u);
  EXPECT_EQ(idx.postings("bar").size(), 1u);
  EXPECT_TRUE(idx.postings("zzz").empty());
}

TEST(InvertedIndexTest, ServesMultipleDocs) {
  InvertedIndex idx;
  auto a = idx.add_document({"/a", 0, 0});
  auto b = idx.add_document({"/b", 0, 0});
  idx.add_term(a, "shared");
  idx.add_term(b, "shared");
  EXPECT_EQ(idx.postings("shared").size(), 2u);
}

TEST(InvertedIndexTest, DocCountAndTermCount) {
  InvertedIndex idx;
  EXPECT_EQ(idx.num_docs(), 0u);
  EXPECT_EQ(idx.num_terms(), 0u);
  auto a = idx.add_document({"/a", 0, 0});
  idx.add_term(a, "x");
  idx.add_term(a, "y");
  EXPECT_EQ(idx.num_docs(), 1u);
  EXPECT_EQ(idx.num_terms(), 2u);
}
