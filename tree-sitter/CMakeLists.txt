cmake_minimum_required(VERSION 3.13)
project(tree_sitter)

add_library(tree_sitter STATIC)
target_sources(tree_sitter PRIVATE
  vendor/tree-sitter/lib/src/lib.c
)
target_include_directories(tree_sitter PUBLIC
  vendor/tree-sitter/lib/src
  vendor/tree-sitter/lib/include
)
target_compile_features(tree_sitter PUBLIC c_std_99)
