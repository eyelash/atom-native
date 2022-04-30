cmake_minimum_required(VERSION 3.13)
project(text-buffer)

add_library(text-buffer)
target_link_libraries(text-buffer superstring_core)
target_sources(text-buffer PRIVATE
  src/display-layer.cc
  src/display-marker-layer.cc
  src/display-marker.cc
  src/helpers.cc
  src/is-character-pair.cc
  src/marker-layer.cc
  src/marker.cc
  src/point.cc
  src/point-helpers.cc
  src/range.cc
  src/screen-line-builder.cc
  src/text-buffer.cc
)
target_include_directories(text-buffer INTERFACE src)
