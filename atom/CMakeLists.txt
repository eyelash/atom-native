cmake_minimum_required(VERSION 3.13)
project(atom)

add_library(atom)
target_link_libraries(atom text-buffer tree-sitter)
target_sources(atom PRIVATE
  src/bracket-matcher-view.cc
  src/bracket-matcher.cc
  src/clipboard.cc
  src/cursor.cc
  src/decoration-manager.cc
  src/decoration.cc
  src/grammar-registry.cc
  src/grammar.cc
  src/layer-decoration.cc
  src/match-manager.cc
  src/null-grammar.cc
  src/select-next.cc
  src/selection.cc
  src/syntax-scope-map.cc
  src/text-editor.cc
  src/text-mate-language-mode.cc
  src/tree-sitter-grammar.cc
  src/tree-sitter-language-mode.cc
  src/whitespace.cc
)
target_include_directories(atom INTERFACE
  src
)
