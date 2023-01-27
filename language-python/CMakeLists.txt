cmake_minimum_required(VERSION 3.13)
project(language-python)

add_library(tree-sitter-python STATIC)
target_compile_features(tree-sitter-python PRIVATE c_std_99)
target_sources(tree-sitter-python PRIVATE
  tree-sitter-python/src/parser.c
  tree-sitter-python/src/scanner.cc
)
target_include_directories(tree-sitter-python PRIVATE
  tree-sitter-python/src
)

add_library(language-python STATIC)
target_link_libraries(language-python atom tree-sitter-python)
target_sources(language-python PRIVATE
  grammars/tree-sitter-python.cc
)
