cmake_minimum_required(VERSION 3.13)
project(language-html)

add_library(tree-sitter-html STATIC)
target_compile_features(tree-sitter-html PRIVATE c_std_99)
target_sources(tree-sitter-html PRIVATE
  vendor/tree-sitter-html/src/parser.c
  vendor/tree-sitter-html/src/scanner.cc
)
target_include_directories(tree-sitter-html PRIVATE
  vendor/tree-sitter-html/src
)

add_library(language-html STATIC)
target_link_libraries(language-html atom tree-sitter-html)
target_sources(language-html PRIVATE
  grammars/tree-sitter-html.cc
)
