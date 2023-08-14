text_buffer = declare_dependency(
  link_with: static_library(
    'text-buffer',
    files(
      'src/default-history-provider.cc',
      'src/display-layer.cc',
      'src/display-marker-layer.cc',
      'src/display-marker.cc',
      'src/file.cc',
      'src/fs-plus.cc',
      'src/helpers.cc',
      'src/is-character-pair.cc',
      'src/language-mode.cc',
      'src/marker-layer.cc',
      'src/marker.cc',
      'src/path.cc',
      'src/point-helpers.cc',
      'src/point.cc',
      'src/range.cc',
      'src/screen-line-builder.cc',
      'src/text-buffer.cc',
    ),
    dependencies: [superstring],
  ),
  include_directories: include_directories(
    'src',
  ),
  dependencies: [superstring],
)
