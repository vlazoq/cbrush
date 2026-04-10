cmake_minimum_required(VERSION 3.22)
project(cbrush_common_utils CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
include(cbrush_defaults)

add_library(cbrush_common_utils
  src/logger.cpp
  src/config.cpp
  src/string_utils.cpp
)
add_library(cbrush::common-utils ALIAS cbrush_common_utils)

target_include_directories(cbrush_common_utils PUBLIC include)
target_compile_features(cbrush_common_utils PUBLIC cxx_std_20)
cbrush_apply_compile_flags(cbrush_common_utils)

if(BUILD_TESTING)
  enable_testing()
  # tests are built only when this library is the top-level project.
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(tests)
  endif()
endif()
