#include <gtest/gtest.h>
#include "fsearch/tokenizer.h"

using namespace fsearch;

TEST(TokenizerTest, LowercasesAndSplitsOnPunctuation) {
  Tokenizer t;
  auto v = t.tokenize("Hello, World! HTTP/1.1 GET /index.html");
  ASSERT_GE(v.size(), 5u);
  EXPECT_EQ(v[0], "hello");
  EXPECT_EQ(v[1], "world");
}

TEST(TokenizerTest, RespectsMinLen) {
  Tokenizer t;
  t.set_min_len(4);
  auto v = t.tokenize("a bb ccc dddd");
  ASSERT_EQ(v.size(), 1u);
  EXPECT_EQ(v[0], "dddd");
}

TEST(TokenizerTest, EmptyStringYieldsEmpty) {
  Tokenizer t;
  EXPECT_TRUE(t.tokenize("").empty());
}

TEST(TokenizerTest, MaxLenClipsLongTokens) {
  Tokenizer t;
  t.set_max_len(3);
  auto v = t.tokenize("one twoo three");
  // "one" passes (len 3), "twoo" (4) and "three" (5) are dropped.
  ASSERT_EQ(v.size(), 1u);
  EXPECT_EQ(v[0], "one");
}
