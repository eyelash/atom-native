tree_sitter = declare_dependency(
  link_with: static_library(
    'tree-sitter',
    files(
      'src/parser.cc',
      'src/tree-cursor.cc',
      'src/tree.cc',
    ),
    include_directories: include_directories(
      'vendor/tree-sitter/lib/include',
    ),
    dependencies: [text_buffer],
    link_with: static_library(
      'tree-sitter-lib',
      'vendor/tree-sitter/lib/src/lib.c',
      include_directories: include_directories(
        'vendor/tree-sitter/lib/src',
        'vendor/tree-sitter/lib/include',
      ),
    ),
  ),
  include_directories: include_directories(
    'vendor/tree-sitter/lib/include',
    'src',
  ),
  dependencies: [text_buffer],
)
