cmake_minimum_required(VERSION 3.13)
project(text-buffer)

add_library(text-buffer)
target_link_libraries(text-buffer superstring_core)
target_sources(text-buffer PRIVATE
  src/text-buffer.cc
)
target_include_directories(text-buffer INTERFACE src)
