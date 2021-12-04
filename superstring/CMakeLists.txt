cmake_minimum_required(VERSION 3.13)
project(superstring)

add_library(superstring_core STATIC)
target_link_libraries(superstring_core pcre)
target_sources(superstring_core PRIVATE
	src/core/encoding-conversion.cc
  src/core/marker-index.cc
  src/core/native-text-buffer.cc
  src/core/patch.cc
  src/core/point.cc
  src/core/range.cc
  src/core/regex.cc
  src/core/text.cc
  src/core/text-slice.cc
  src/core/text-diff.cc
  src/core/libmba-diff.cc
)
target_include_directories(superstring_core PRIVATE vendor/libcxx)
target_include_directories(superstring_core INTERFACE src/core)

add_subdirectory(vendor/pcre)
