include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_common_utils
  test_string_utils.cpp
  test_config.cpp
)
target_link_libraries(test_common_utils PRIVATE cbrush_common_utils GTest::gtest_main)
cbrush_apply_compile_flags(test_common_utils)

include(GoogleTest)
gtest_discover_tests(test_common_utils)
