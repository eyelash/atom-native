atom_files = files(
  'src/cursor.cc',
  'src/decoration.cc',
  'src/decoration-manager.cc',
  'src/layer-decoration.cc',
  'src/selection.cc',
  'src/select-next.cc',
  'src/text-editor.cc',
)
atom_include_directories = [include_directories(
  'src',
)]
atom_library = static_library('atom', atom_files, include_directories: atom_include_directories, dependencies: text_buffer_dependency)
atom_dependency = declare_dependency(link_with: atom_library, include_directories: atom_include_directories + text_buffer_include_directories + superstring_core_include_directories)
