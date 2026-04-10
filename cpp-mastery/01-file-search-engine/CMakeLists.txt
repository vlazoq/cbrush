cmake_minimum_required(VERSION 3.22)
project(fsearch CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../shared/cmake")
include(cbrush_defaults)

add_library(fsearch_lib
  src/walker.cpp
  src/tokenizer.cpp
  src/inverted_index.cpp
  src/index_store.cpp
  src/query_engine.cpp
  src/cli.cpp
)
target_include_directories(fsearch_lib PUBLIC include)
target_compile_features(fsearch_lib PUBLIC cxx_std_20)
cbrush_apply_compile_flags(fsearch_lib)

add_executable(fsearch src/main.cpp)
target_link_libraries(fsearch PRIVATE fsearch_lib)
cbrush_apply_compile_flags(fsearch)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
