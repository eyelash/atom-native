superstring = declare_dependency(
  link_with: static_library(
    'superstring',
    files(
      'src/encoding-conversion.cc',
      'src/libmba-diff.cc',
      'src/marker-index.cc',
      'src/native-point.cc',
      'src/native-range.cc',
      'src/native-text-buffer.cc',
      'src/patch.cc',
      'src/regex.cc',
      'src/text-diff.cc',
      'src/text-slice.cc',
      'src/text.cc',
    ),
    include_directories: include_directories(
      'vendor/libcxx',
    ),
    dependencies: [dependency('libpcre2-16')],
    cpp_args: ['-DPCRE2_CODE_UNIT_WIDTH=16'],
  ),
  include_directories: include_directories(
    'src',
  ),
)
