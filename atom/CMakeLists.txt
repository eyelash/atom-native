cmake_minimum_required(VERSION 3.13)
project(atom)

add_library(atom)
target_link_libraries(atom text-buffer oniguruma libgit2)
target_sources(atom PRIVATE
  src/cursor.cc
  src/decoration.cc
  src/decoration-manager.cc
  src/layer-decoration.cc
  src/selection.cc
  src/text-editor.cc
)
target_include_directories(atom INTERFACE src)
