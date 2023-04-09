language_rust = declare_dependency(
  link_with: static_library(
    'language-rust',
    'grammars/tree-sitter-rust.cc',
    link_with: static_library(
      'tree-sitter-rust',
      'tree-sitter-rust/src/parser.c',
      'tree-sitter-rust/src/scanner.c',
      include_directories: include_directories(
        'tree-sitter-rust/src',
      ),
    ),
    dependencies: [atom],
  )
)
