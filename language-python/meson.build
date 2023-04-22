language_python = declare_dependency(
  link_with: static_library(
    'language-python',
    'grammars/tree-sitter-python.cc',
    link_with: static_library(
      'tree-sitter-python',
      'vendor/tree-sitter-python/src/parser.c',
      'vendor/tree-sitter-python/src/scanner.cc',
      include_directories: include_directories(
        'vendor/tree-sitter-python/src',
      ),
    ),
    dependencies: [atom],
  )
)
