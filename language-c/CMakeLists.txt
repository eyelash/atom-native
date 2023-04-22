cmake_minimum_required(VERSION 3.13)
project(language-c)

add_library(tree-sitter-c STATIC)
target_compile_features(tree-sitter-c PRIVATE c_std_99)
target_sources(tree-sitter-c PRIVATE
  vendor/tree-sitter-c/src/parser.c
)
target_include_directories(tree-sitter-c PRIVATE
  vendor/tree-sitter-c/src
)

add_library(tree-sitter-cpp STATIC)
target_compile_features(tree-sitter-cpp PRIVATE c_std_99)
target_sources(tree-sitter-cpp PRIVATE
  vendor/tree-sitter-cpp/src/parser.c
  vendor/tree-sitter-cpp/src/scanner.cc
)
target_include_directories(tree-sitter-cpp PRIVATE
  vendor/tree-sitter-cpp/src
)

add_library(language-c STATIC)
target_link_libraries(language-c atom tree-sitter-c tree-sitter-cpp)
target_sources(language-c PRIVATE
  grammars/tree-sitter-c.cc
  grammars/tree-sitter-cpp.cc
)
