project('atom-native', 'cpp', 'c')

subdir('superstring')
subdir('text-buffer')
subdir('tree-sitter')
subdir('atom')
subdir('language-c')
subdir('language-go')
subdir('language-javascript')
subdir('language-python')
subdir('language-rust')

atom_dep = declare_dependency(
  dependencies: [atom, language_c, language_go, language_javascript, language_python, language_rust]
)
