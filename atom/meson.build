atom = declare_dependency(
  link_with: static_library(
    'atom',
    files(
      'src/bracket-matcher.cc',
      'src/clipboard.cc',
      'src/cursor.cc',
      'src/decoration-manager.cc',
      'src/decoration.cc',
      'src/grammar-registry.cc',
      'src/grammar.cc',
      'src/layer-decoration.cc',
      'src/null-grammar.cc',
      'src/select-next.cc',
      'src/selection.cc',
      'src/syntax-scope-map.cc',
      'src/text-editor.cc',
      'src/text-mate-language-mode.cc',
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
