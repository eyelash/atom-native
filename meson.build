project('atom-native', 'cpp', 'c')

subdir('superstring')
subdir('text-buffer')
subdir('tree-sitter')
subdir('atom')
subdir('language-c')
subdir('language-css')
subdir('language-go')
subdir('language-html')
subdir('language-javascript')
subdir('language-python')
subdir('language-rust')

atom_dep = declare_dependency(
  dependencies: [
    atom,
    language_c,
    language_css,
    language_go,
    language_html,
    language_javascript,
    language_python,
    language_rust,
  ]
)
