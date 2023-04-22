cmake_minimum_required(VERSION 3.13)
project(language-javascript)

add_library(tree-sitter-javascript STATIC)
target_compile_features(tree-sitter-javascript PRIVATE c_std_99)
target_sources(tree-sitter-javascript PRIVATE
  vendor/tree-sitter-javascript/src/parser.c
  vendor/tree-sitter-javascript/src/scanner.c
)
target_include_directories(tree-sitter-javascript PRIVATE
  vendor/tree-sitter-javascript/src
)

add_library(language-javascript STATIC)
target_link_libraries(language-javascript atom tree-sitter-javascript)
target_sources(language-javascript PRIVATE
  grammars/tree-sitter-javascript.cc
)
