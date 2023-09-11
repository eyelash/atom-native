cmake_minimum_required(VERSION 3.13)
project(superstring)

add_library(superstring STATIC)
target_compile_features(superstring PUBLIC cxx_std_11)
target_link_libraries(superstring
  pcre2
)
target_sources(superstring PRIVATE
  src/encoding-conversion.cc
  src/libmba-diff.cc
  src/marker-index.cc
  src/native-point.cc
  src/native-range.cc
  src/native-text-buffer.cc
  src/patch.cc
  src/regex.cc
  src/text-diff.cc
  src/text-slice.cc
  src/text.cc
)
target_include_directories(superstring PRIVATE
  vendor/libcxx
)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_link_libraries(superstring
    iconv
  )
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(superstring PRIVATE
    vendor/win-iconv/win_iconv.c
  )
  target_include_directories(superstring PRIVATE
    vendor/win-iconv
  )
  target_compile_definitions(superstring PRIVATE
    WINICONV_CONST=
    PCRE2_STATIC
  )
endif()
target_include_directories(superstring INTERFACE
  src
)

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
target_include_directories(tests PRIVATE
  src
)
find_package(Threads REQUIRED)
target_link_libraries(tests
  superstring
  catch
  Threads::Threads
)
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE
  vendor
)

add_subdirectory(vendor/pcre2)
