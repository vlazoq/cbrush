#include <gtest/gtest.h>
#include "fsearch/inverted_index.h"
#include "fsearch/query_engine.h"

using namespace fsearch;

namespace {
InvertedIndex build() {
  InvertedIndex idx;
  auto a = idx.add_document({"/notes/arch.md", 0, 0});
  idx.add_term(a, "architecture");
  idx.add_term(a, "design");
  auto b = idx.add_document({"/notes/meeting.txt", 0, 0});
  idx.add_term(b, "meeting");
  idx.add_term(b, "design");
  return idx;
}
}  // namespace

TEST(QueryEngineTest, SingleTerm) {
  auto idx = build();
  QueryEngine q(idx);
  auto h = q.search("architecture");
  ASSERT_EQ(h.size(), 1u);
  EXPECT_EQ(idx.document(h[0].doc).path, "/notes/arch.md");
}

TEST(QueryEngineTest, AndAcrossTerms) {
  auto idx = build();
  QueryEngine q(idx);
  EXPECT_EQ(q.search("design meeting").size(), 1u);
  EXPECT_EQ(q.search("design architecture").size(), 1u);
  EXPECT_TRUE(q.search("architecture meeting").empty());
}

TEST(QueryEngineTest, GlobFilter) {
  auto idx = build();
  QueryEngine q(idx);
  auto h = q.search_with_glob("design", "*.md");
  ASSERT_EQ(h.size(), 1u);
  EXPECT_EQ(idx.document(h[0].doc).path, "/notes/arch.md");
}

TEST(QueryEngineTest, EmptyQueryReturnsEmpty) {
  auto idx = build();
  QueryEngine q(idx);
  EXPECT_TRUE(q.search("").empty());
}
