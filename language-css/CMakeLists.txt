cmake_minimum_required(VERSION 3.13)
project(language-css)

add_library(tree-sitter-css STATIC)
target_compile_features(tree-sitter-css PRIVATE c_std_99)
target_sources(tree-sitter-css PRIVATE
  vendor/tree-sitter-css/src/parser.c
  vendor/tree-sitter-css/src/scanner.c
)
target_include_directories(tree-sitter-css PRIVATE
  vendor/tree-sitter-css/src
)

add_library(language-css STATIC)
target_link_libraries(language-css atom tree-sitter-css)
target_sources(language-css PRIVATE
  grammars/tree-sitter-css.cc
)
