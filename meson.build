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
      'text-buffer/src/path.cc',
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
      'src/bracket-matcher.cc',
      'src/cursor.cc',
      'src/decoration-manager.cc',
      'src/decoration.cc',
      'src/grammar-registry.cc',
      'src/layer-decoration.cc',
      'src/select-next.cc',
      'src/selection.cc',
      'src/syntax-scope-map.cc',
      'src/text-editor.cc',
      'src/tree-sitter-grammar.cc',
      'src/tree-sitter-language-mode.cc',
    ),
    dependencies: [text_buffer, tree_sitter],
  ),
  include_directories: include_directories(
    'src',
  ),
  dependencies: [text_buffer, tree_sitter],
)

subdir('language-c')
subdir('language-go')
subdir('language-javascript')
subdir('language-python')
subdir('language-rust')

atom_dep = declare_dependency(
  dependencies: [atom, language_c, language_go, language_javascript, language_python, language_rust]
)
