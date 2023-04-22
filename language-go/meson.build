language_go = declare_dependency(
  link_with: static_library(
    'language-go',
    'grammars/tree-sitter-go.cc',
    link_with: static_library(
      'tree-sitter-go',
      'vendor/tree-sitter-go/src/parser.c',
      include_directories: include_directories(
        'vendor/tree-sitter-go/src',
      ),
    ),
    dependencies: [atom],
  )
)
