#include <gtest/gtest.h>
#include "cbrush/config.h"

TEST(ConfigTest, ParsesKeyValue) {
  auto c = cbrush::Config::from_string("port = 1234\nhost = localhost\n");
  EXPECT_EQ(c.get_int("port", 0), 1234);
  EXPECT_EQ(c.get_or("host", ""), "localhost");
}

TEST(ConfigTest, SupportsSections) {
  auto c = cbrush::Config::from_string("[net]\nport=80\n[log]\nlevel=debug\n");
  EXPECT_EQ(c.get_int("net.port", 0), 80);
  EXPECT_EQ(c.get_or("log.level", ""), "debug");
}

TEST(ConfigTest, IgnoresComments) {
  auto c = cbrush::Config::from_string("# comment\nport=7\n");
  EXPECT_EQ(c.get_int("port", 0), 7);
}

TEST(ConfigTest, BoolParsing) {
  auto c = cbrush::Config::from_string("a=yes\nb=on\nc=nope\n");
  EXPECT_TRUE(c.get_bool("a", false));
  EXPECT_TRUE(c.get_bool("b", false));
  EXPECT_FALSE(c.get_bool("c", false));
}
