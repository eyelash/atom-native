cmake_minimum_required(VERSION 3.13)
project(tree-sitter)

add_library(tree-sitter-lib STATIC)
target_compile_features(tree-sitter-lib PRIVATE c_std_99)
target_sources(tree-sitter-lib PRIVATE
  vendor/tree-sitter/lib/src/lib.c
)
target_include_directories(tree-sitter-lib PRIVATE
  vendor/tree-sitter/lib/src
  vendor/tree-sitter/lib/include
)
target_include_directories(tree-sitter-lib INTERFACE
  vendor/tree-sitter/lib/include
)

add_library(tree-sitter STATIC)
target_link_libraries(tree-sitter tree-sitter-lib superstring)
target_sources(tree-sitter PRIVATE
  src/parser.cc
  src/tree-cursor.cc
  src/tree-sitter.cc
  src/tree.cc
)
target_include_directories(tree-sitter INTERFACE
  src
)
