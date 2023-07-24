language_html = declare_dependency(
  link_with: static_library(
    'language-html',
    'grammars/tree-sitter-html.cc',
    link_with: static_library(
      'tree-sitter-html',
      'vendor/tree-sitter-html/src/parser.c',
      'vendor/tree-sitter-html/src/scanner.cc',
      include_directories: include_directories(
        'vendor/tree-sitter-html/src',
      ),
    ),
    dependencies: [atom],
  )
)
