pcre_sources = files(
  'pcre2_chartables.c',
  '10.23/src/pcre2_auto_possess.c',
  '10.23/src/pcre2_compile.c',
  '10.23/src/pcre2_config.c',
  '10.23/src/pcre2_context.c',
  '10.23/src/pcre2_dfa_match.c',
  '10.23/src/pcre2_error.c',
  '10.23/src/pcre2_find_bracket.c',
  '10.23/src/pcre2_jit_compile.c',
  '10.23/src/pcre2_maketables.c',
  '10.23/src/pcre2_match.c',
  '10.23/src/pcre2_match_data.c',
  '10.23/src/pcre2_newline.c',
  '10.23/src/pcre2_ord2utf.c',
  '10.23/src/pcre2_pattern_info.c',
  '10.23/src/pcre2_serialize.c',
  '10.23/src/pcre2_string_utils.c',
  '10.23/src/pcre2_study.c',
  '10.23/src/pcre2_substitute.c',
  '10.23/src/pcre2_substring.c',
  '10.23/src/pcre2_tables.c',
  '10.23/src/pcre2_ucd.c',
  '10.23/src/pcre2_valid_utf.c',
  '10.23/src/pcre2_xclass.c',
)
pcre_include_directories = [include_directories(
  'include',
  '10.23/src',
)]
pcre_defines = [
  '-DHAVE_CONFIG_H',
  '-DPCRE2_CODE_UNIT_WIDTH=16',
  '-DSUPPORT_JIT',
]
pcre_library = static_library('pcre', pcre_sources, include_directories: pcre_include_directories, c_args: pcre_defines)
pcre_dependency = declare_dependency(link_with: pcre_library, include_directories: pcre_include_directories, compile_args: pcre_defines)
