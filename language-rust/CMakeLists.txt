cmake_minimum_required(VERSION 3.13)
project(language-rust)

add_library(tree-sitter-rust STATIC)
target_compile_features(tree-sitter-rust PRIVATE c_std_99)
target_sources(tree-sitter-rust PRIVATE
  vendor/tree-sitter-rust/src/parser.c
  vendor/tree-sitter-rust/src/scanner.c
)
target_include_directories(tree-sitter-rust PRIVATE
  vendor/tree-sitter-rust/src
)

add_library(language-rust STATIC)
target_link_libraries(language-rust atom tree-sitter-rust)
target_sources(language-rust PRIVATE
  grammars/tree-sitter-rust.cc
)
