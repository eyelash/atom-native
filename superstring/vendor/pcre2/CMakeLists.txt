cmake_minimum_required(VERSION 3.13)
project(pcre2)

add_library(pcre2 STATIC)
target_sources(pcre2 PRIVATE
  pcre2_chartables.c
  10.23/src/pcre2_auto_possess.c
  10.23/src/pcre2_compile.c
  10.23/src/pcre2_config.c
  10.23/src/pcre2_context.c
  10.23/src/pcre2_dfa_match.c
  10.23/src/pcre2_error.c
  10.23/src/pcre2_find_bracket.c
  10.23/src/pcre2_jit_compile.c
  10.23/src/pcre2_maketables.c
  10.23/src/pcre2_match.c
  10.23/src/pcre2_match_data.c
  10.23/src/pcre2_newline.c
  10.23/src/pcre2_ord2utf.c
  10.23/src/pcre2_pattern_info.c
  10.23/src/pcre2_serialize.c
  10.23/src/pcre2_string_utils.c
  10.23/src/pcre2_study.c
  10.23/src/pcre2_substitute.c
  10.23/src/pcre2_substring.c
  10.23/src/pcre2_tables.c
  10.23/src/pcre2_ucd.c
  10.23/src/pcre2_valid_utf.c
  10.23/src/pcre2_xclass.c
)
target_include_directories(pcre2 PRIVATE
  include
  10.23/src
)
target_compile_definitions(pcre2 PRIVATE
  HAVE_CONFIG_H
  PCRE2_CODE_UNIT_WIDTH=16
  SUPPORT_JIT
)
target_include_directories(pcre2 INTERFACE
  include
)
target_compile_definitions(pcre2 INTERFACE
  PCRE2_CODE_UNIT_WIDTH=16
)
