#include <gtest/gtest.h>
#include "fsearch/cli.h"

using namespace fsearch;

TEST(CliTest, ParseArgsIndex) {
  auto o = parse_args({"fsearch", "index", "--root", "/tmp", "--db", "x.db"});
  EXPECT_EQ(o.command, "index");
  EXPECT_EQ(o.root, "/tmp");
  EXPECT_EQ(o.db, "x.db");
}

TEST(CliTest, ParseArgsQueryWithGlobAndFreeTerm) {
  auto o = parse_args({"fsearch", "query", "--glob", "*.md", "needle"});
  EXPECT_EQ(o.command, "query");
  EXPECT_EQ(o.glob, "*.md");
  EXPECT_EQ(o.query, "needle");
}

TEST(CliTest, ParseArgsMultiwordQuery) {
  auto o = parse_args({"fsearch", "query", "foo", "bar", "baz"});
  EXPECT_EQ(o.query, "foo bar baz");
}

TEST(CliTest, ParseArgsUnknownFlagThrows) {
  EXPECT_THROW(parse_args({"fsearch", "query", "--nope"}), std::runtime_error);
}
