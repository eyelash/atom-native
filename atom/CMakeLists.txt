cmake_minimum_required(VERSION 3.13)
project(atom)

add_library(atom)
target_link_libraries(atom text-buffer tree-sitter)
target_sources(atom PRIVATE
  src/bracket-matcher.cc
  src/clipboard.cc
  src/cursor.cc
  src/decoration-manager.cc
  src/decoration.cc
  src/grammar-registry.cc
  src/layer-decoration.cc
  src/select-next.cc
  src/selection.cc
  src/syntax-scope-map.cc
  src/text-editor.cc
  src/tree-sitter-grammar.cc
  src/tree-sitter-language-mode.cc
)
target_include_directories(atom INTERFACE
  src
)
