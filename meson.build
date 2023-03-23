project('atom-native', 'cpp', 'c')

superstring = declare_dependency(
  link_with: static_library(
    'superstring',
    files(
      'superstring/src/encoding-conversion.cc',
      'superstring/src/libmba-diff.cc',
      'superstring/src/marker-index.cc',
      'superstring/src/native-point.cc',
      'superstring/src/native-range.cc',
      'superstring/src/native-text-buffer.cc',
      'superstring/src/patch.cc',
      'superstring/src/regex.cc',
      'superstring/src/text-diff.cc',
      'superstring/src/text-slice.cc',
      'superstring/src/text.cc',
    ),
    include_directories: include_directories(
      'superstring/vendor/libcxx',
    ),
    dependencies: [dependency('libpcre2-16')],
    cpp_args: ['-DPCRE2_CODE_UNIT_WIDTH=16'],
  ),
  include_directories: include_directories(
    'superstring/src',
  ),
)

text_buffer = declare_dependency(
  link_with: static_library(
    'text-buffer',
    files(
      'text-buffer/src/default-history-provider.cc',
      'text-buffer/src/display-layer.cc',
      'text-buffer/src/display-marker-layer.cc',
      'text-buffer/src/display-marker.cc',
      'text-buffer/src/file.cc',
      'text-buffer/src/helpers.cc',
      'text-buffer/src/is-character-pair.cc',
      'text-buffer/src/language-mode.cc',
      'text-buffer/src/marker-layer.cc',
      'text-buffer/src/marker.cc',
      'text-buffer/src/point-helpers.cc',
      'text-buffer/src/point.cc',
      'text-buffer/src/range.cc',
      'text-buffer/src/screen-line-builder.cc',
      'text-buffer/src/text-buffer.cc',
    ),
    dependencies: [superstring],
  ),
  include_directories: include_directories(
    'text-buffer/src',
  ),
  dependencies: [superstring],
)

tree_sitter = declare_dependency(
  link_with: static_library(
    'tree-sitter',
    files(
      'tree-sitter/src/parser.cc',
      'tree-sitter/src/tree-cursor.cc',
      'tree-sitter/src/tree.cc',
    ),
    include_directories: include_directories(
      'tree-sitter/vendor/tree-sitter/lib/include',
    ),
    dependencies: [text_buffer],
    link_with: static_library(
      'tree-sitter-lib',
      'tree-sitter/vendor/tree-sitter/lib/src/lib.c',
      include_directories: include_directories(
        'tree-sitter/vendor/tree-sitter/lib/src',
        'tree-sitter/vendor/tree-sitter/lib/include',
      ),
    ),
  ),
  include_directories: include_directories(
    'tree-sitter/vendor/tree-sitter/lib/include',
    'tree-sitter/src',
  ),
  dependencies: [text_buffer],
)

atom = declare_dependency(
  link_with: static_library(
    'atom',
    files(
      'atom/src/bracket-matcher.cc',
      'atom/src/cursor.cc',
      'atom/src/decoration-manager.cc',
      'atom/src/decoration.cc',
      'atom/src/grammar-registry.cc',
      'atom/src/layer-decoration.cc',
      'atom/src/select-next.cc',
      'atom/src/selection.cc',
      'atom/src/syntax-scope-map.cc',
      'atom/src/text-editor.cc',
      'atom/src/tree-sitter-grammar.cc',
      'atom/src/tree-sitter-language-mode.cc',
    ),
    dependencies: [text_buffer, tree_sitter],
  ),
  include_directories: include_directories(
    'atom/src',
  ),
  dependencies: [text_buffer, tree_sitter],
)

language_c = declare_dependency(
  link_with: static_library(
    'language-c',
    'language-c/grammars/tree-sitter-c.cc',
    'language-c/grammars/tree-sitter-cpp.cc',
    link_with: [
      static_library(
        'tree-sitter-c',
        'language-c/tree-sitter-c/src/parser.c',
        include_directories: include_directories(
          'language-c/tree-sitter-c/src',
        ),
      ),
      static_library(
        'tree-sitter-cpp',
        'language-c/tree-sitter-cpp/src/parser.c',
        'language-c/tree-sitter-cpp/src/scanner.cc',
        include_directories: include_directories(
          'language-c/tree-sitter-cpp/src',
        ),
      ),
    ],
    dependencies: [atom],
  )
)

language_javascript = declare_dependency(
  link_with: static_library(
    'language-javascript',
    'language-javascript/grammars/tree-sitter-javascript.cc',
    link_with: static_library(
      'tree-sitter-javascript',
      'language-javascript/tree-sitter-javascript/src/parser.c',
      'language-javascript/tree-sitter-javascript/src/scanner.c',
      include_directories: include_directories(
        'language-javascript/tree-sitter-javascript/src',
      ),
    ),
    dependencies: [atom],
  )
)

language_python = declare_dependency(
  link_with: static_library(
    'language-python',
    'language-python/grammars/tree-sitter-python.cc',
    link_with: static_library(
      'tree-sitter-python',
      'language-python/tree-sitter-python/src/parser.c',
      'language-python/tree-sitter-python/src/scanner.cc',
      include_directories: include_directories(
        'language-python/tree-sitter-python/src',
      ),
    ),
    dependencies: [atom],
  )
)

atom_dep = declare_dependency(
  dependencies: [atom, language_c, language_javascript, language_python]
)
