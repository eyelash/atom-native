language_css = declare_dependency(
  link_with: static_library(
    'language-css',
    'grammars/tree-sitter-css.cc',
    link_with: static_library(
      'tree-sitter-css',
      'vendor/tree-sitter-css/src/parser.c',
      'vendor/tree-sitter-css/src/scanner.c',
      include_directories: include_directories(
        'vendor/tree-sitter-css/src',
      ),
    ),
    dependencies: [atom],
  )
)
