language_javascript = declare_dependency(
  link_with: static_library(
    'language-javascript',
    'grammars/tree-sitter-javascript.cc',
    link_with: static_library(
      'tree-sitter-javascript',
      'tree-sitter-javascript/src/parser.c',
      'tree-sitter-javascript/src/scanner.c',
      include_directories: include_directories(
        'tree-sitter-javascript/src',
      ),
    ),
    dependencies: [atom],
  )
)
