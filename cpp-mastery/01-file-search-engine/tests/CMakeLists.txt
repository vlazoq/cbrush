include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# Prevent GoogleTest from overriding parent CMake vars on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_fsearch
  test_tokenizer.cpp
  test_inverted_index.cpp
  test_index_store.cpp
  test_query_engine.cpp
  test_walker.cpp
  test_cli.cpp
)
target_link_libraries(test_fsearch PRIVATE fsearch_lib GTest::gtest_main)
cbrush_apply_compile_flags(test_fsearch)

include(GoogleTest)
gtest_discover_tests(test_fsearch)
