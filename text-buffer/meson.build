text_buffer_files = files(
  'src/display-layer.cc',
  'src/display-marker-layer.cc',
  'src/display-marker.cc',
  'src/helpers.cc',
  'src/is-character-pair.cc',
  'src/language-mode.cc',
  'src/marker-layer.cc',
  'src/marker.cc',
  'src/point.cc',
  'src/point-helpers.cc',
  'src/range.cc',
  'src/screen-line-builder.cc',
  'src/text-buffer.cc',
)
text_buffer_include_directories = [include_directories(
  'src',
)]
text_buffer_library = static_library('text-buffer', text_buffer_files, include_directories: text_buffer_include_directories, dependencies: superstring_core_dependency)
text_buffer_dependency = declare_dependency(link_with: text_buffer_library, include_directories: text_buffer_include_directories + superstring_core_include_directories)
