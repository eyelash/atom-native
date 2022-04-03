cmake_minimum_required(VERSION 3.13)
project(superstring)

add_library(superstring_core STATIC)
target_compile_features(superstring_core PUBLIC cxx_std_11)
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

add_subdirectory(vendor/pcre)
