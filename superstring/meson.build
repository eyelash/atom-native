subdir('vendor/pcre')

superstring_core_files = files(
  'src/core/encoding-conversion.cc',
  'src/core/marker-index.cc',
  'src/core/native-point.cc',
  'src/core/native-range.cc',
  'src/core/native-text-buffer.cc',
  'src/core/patch.cc',
  'src/core/regex.cc',
  'src/core/text.cc',
  'src/core/text-slice.cc',
  'src/core/text-diff.cc',
  'src/core/libmba-diff.cc',
)
superstring_core_include_directories = [include_directories(
  'vendor/libcxx',
  'src/core',
)]
superstring_core_defines = []
if build_machine.system() == 'windows'
  superstring_core_files += files('vendor/win-iconv/win_iconv.c')
  superstring_core_include_directories += include_directories('vendor/win-iconv')
  superstring_core_defines += ['-DWINICONV_CONST=', '-DPCRE2_STATIC']
endif
superstring_core_library = static_library('superstring_core', superstring_core_files, include_directories: superstring_core_include_directories, dependencies: pcre_dependency, c_args: superstring_core_defines, cpp_args: superstring_core_defines)
superstring_core_dependency = declare_dependency(link_with: superstring_core_library, include_directories: superstring_core_include_directories, compile_args: superstring_core_defines)
