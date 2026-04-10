#pragma once
#include <string>
#include <vector>

namespace fsearch {

struct CliOptions {
  std::string command;            // "index" | "query"
  std::string root;               // for index
  std::string db = "fsearch.db";  // both
  std::string query;              // for query
  std::string glob;               // optional
  std::size_t limit = 25;
};

int run_cli(int argc, char** argv);
CliOptions parse_args(const std::vector<std::string>& argv);

}  // namespace fsearch
