cmake_minimum_required(VERSION 3.13)
project(language-json)

add_library(tree-sitter-json STATIC)
target_compile_features(tree-sitter-json PRIVATE c_std_99)
target_sources(tree-sitter-json PRIVATE
  vendor/tree-sitter-json/src/parser.c
)
target_include_directories(tree-sitter-json PRIVATE
  vendor/tree-sitter-json/src
)

add_library(language-json STATIC)
target_link_libraries(language-json atom tree-sitter-json)
target_sources(language-json PRIVATE
  grammars/tree-sitter-json.cc
)
