language_c = declare_dependency(
  link_with: static_library(
    'language-c',
    'grammars/tree-sitter-c.cc',
    'grammars/tree-sitter-cpp.cc',
    link_with: [
      static_library(
        'tree-sitter-c',
        'vendor/tree-sitter-c/src/parser.c',
        include_directories: include_directories(
          'vendor/tree-sitter-c/src',
        ),
      ),
      static_library(
        'tree-sitter-cpp',
        'vendor/tree-sitter-cpp/src/parser.c',
        'vendor/tree-sitter-cpp/src/scanner.cc',
        include_directories: include_directories(
          'vendor/tree-sitter-cpp/src',
        ),
      ),
    ],
    dependencies: [atom],
  )
)
