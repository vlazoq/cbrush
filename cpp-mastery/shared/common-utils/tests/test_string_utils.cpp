#include <gtest/gtest.h>
#include "cbrush/string_utils.h"

using namespace cbrush::strings;

TEST(StringUtilsTest, SplitBasic) {
  auto v = split("a,b,c", ',');
  ASSERT_EQ(v.size(), 3u);
  EXPECT_EQ(v[0], "a");
  EXPECT_EQ(v[2], "c");
}

TEST(StringUtilsTest, Trim) {
  EXPECT_EQ(trim("   hi  \t"), "hi");
  EXPECT_TRUE(trim("").empty());
}

TEST(StringUtilsTest, LowerUpper) {
  EXPECT_EQ(to_lower("AbC"), "abc");
  EXPECT_EQ(to_upper("AbC"), "ABC");
}

TEST(StringUtilsTest, StartsEndsWith) {
  EXPECT_TRUE(starts_with("hello world", "hello"));
  EXPECT_TRUE(ends_with("hello world", "world"));
  EXPECT_FALSE(starts_with("hi", "hello"));
}
