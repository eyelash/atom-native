cmake_minimum_required(VERSION 3.13)
project(language-go)

add_library(tree-sitter-go STATIC)
target_compile_features(tree-sitter-go PRIVATE c_std_99)
target_sources(tree-sitter-go PRIVATE
  tree-sitter-go/src/parser.c
)
target_include_directories(tree-sitter-go PRIVATE
  tree-sitter-go/src
)

add_library(language-go STATIC)
target_link_libraries(language-go atom tree-sitter-go)
target_sources(language-go PRIVATE
  grammars/tree-sitter-go.cc
)
