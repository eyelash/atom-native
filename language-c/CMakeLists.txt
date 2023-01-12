cmake_minimum_required(VERSION 3.13)
project(language-c)

add_library(tree-sitter-c STATIC)
target_compile_features(tree-sitter-c PRIVATE c_std_99)
target_sources(tree-sitter-c PRIVATE
  tree-sitter-c/src/parser.c
)
target_include_directories(tree-sitter-c PRIVATE
  tree-sitter-c/src
)
