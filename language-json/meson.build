language_json = declare_dependency(
  link_with: static_library(
    'language-json',
    'grammars/tree-sitter-json.cc',
    link_with: static_library(
      'tree-sitter-json',
      'vendor/tree-sitter-json/src/parser.c',
      include_directories: include_directories(
        'vendor/tree-sitter-json/src',
      ),
    ),
    dependencies: [atom],
  )
)
