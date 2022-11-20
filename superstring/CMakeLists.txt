cmake_minimum_required(VERSION 3.13)
project(superstring)

add_library(superstring_core STATIC)
target_compile_features(superstring_core PUBLIC cxx_std_11)
target_link_libraries(superstring_core
  pcre2
  $<$<PLATFORM_ID:Darwin>:iconv>
)
target_sources(superstring_core PRIVATE
  src/core/encoding-conversion.cc
  src/core/marker-index.cc
  src/core/native-point.cc
  src/core/native-range.cc
  src/core/native-text-buffer.cc
  src/core/patch.cc
  src/core/regex.cc
  src/core/text.cc
  src/core/text-slice.cc
  src/core/text-diff.cc
  src/core/libmba-diff.cc
  $<$<PLATFORM_ID:Windows>:vendor/win-iconv/win_iconv.c>
)
target_include_directories(superstring_core PRIVATE
  vendor/libcxx
  $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/vendor/win-iconv>
)
target_compile_definitions(superstring_core PRIVATE
  $<$<PLATFORM_ID:Windows>:WINICONV_CONST=>
  $<$<PLATFORM_ID:Windows>:PCRE2_STATIC>
)
target_include_directories(superstring_core INTERFACE src/core)

add_executable(tests)
target_sources(tests PRIVATE
  test/native/test-helpers.cc
  test/native/tests.cc
  test/native/encoding-conversion-test.cc
  test/native/native-text-buffer-test.cc
  test/native/patch-test.cc
  test/native/text-test.cc
  test/native/text-diff-test.cc
)
find_package(Threads REQUIRED)
target_link_libraries(tests superstring_core Threads::Threads)
target_include_directories(tests PRIVATE
  vendor
  src/core
)

add_subdirectory(vendor/pcre)
