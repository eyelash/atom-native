cmake_minimum_required(VERSION 3.13)
project(text-buffer)

add_library(text-buffer STATIC)
target_link_libraries(text-buffer superstring)
target_sources(text-buffer PRIVATE
  src/default-history-provider.cc
  src/display-layer.cc
  src/display-marker-layer.cc
  src/display-marker.cc
  src/file.cc
  src/fs-plus.cc
  src/helpers.cc
  src/is-character-pair.cc
  src/language-mode.cc
  src/marker-layer.cc
  src/marker.cc
  src/path.cc
  src/point-helpers.cc
  src/point.cc
  src/range.cc
  src/screen-line-builder.cc
  src/text-buffer.cc
)
target_include_directories(text-buffer INTERFACE
  src
)
