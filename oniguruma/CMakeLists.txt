cmake_minimum_required(VERSION 3.13)
project(oniguruma)

add_library(oniguruma STATIC)
target_sources(oniguruma PRIVATE
  deps/onig/oniggnu.h
  deps/onig/onigposix.h
  deps/onig/oniguruma.h
  deps/onig/regcomp.c
  deps/onig/regenc.c
  deps/onig/regenc.h
  deps/onig/regerror.c
  deps/onig/regexec.c
  deps/onig/regext.c
  deps/onig/reggnu.c
  deps/onig/regint.h
  deps/onig/regparse.c
  deps/onig/regparse.h
  deps/onig/regposerr.c
  deps/onig/regposix.c
  deps/onig/regsyntax.c
  deps/onig/regtrav.c
  deps/onig/regversion.c
  deps/onig/st.c
  deps/onig/st.c
  deps/onig/enc/ascii.c
  deps/onig/enc/big5.c
  deps/onig/enc/cp1251.c
  deps/onig/enc/euc_jp.c
  deps/onig/enc/euc_kr.c
  deps/onig/enc/euc_tw.c
  deps/onig/enc/gb18030.c
  deps/onig/enc/iso8859_1.c
  deps/onig/enc/iso8859_2.c
  deps/onig/enc/iso8859_3.c
  deps/onig/enc/iso8859_4.c
  deps/onig/enc/iso8859_5.c
  deps/onig/enc/iso8859_6.c
  deps/onig/enc/iso8859_7.c
  deps/onig/enc/iso8859_8.c
  deps/onig/enc/iso8859_9.c
  deps/onig/enc/iso8859_10.c
  deps/onig/enc/iso8859_11.c
  deps/onig/enc/iso8859_13.c
  deps/onig/enc/iso8859_14.c
  deps/onig/enc/iso8859_15.c
  deps/onig/enc/iso8859_16.c
  deps/onig/enc/koi8.c
  deps/onig/enc/koi8_r.c
  deps/onig/enc/mktable.c
  deps/onig/enc/sjis.c
  deps/onig/enc/unicode.c
  deps/onig/enc/utf16_be.c
  deps/onig/enc/utf16_le.c
  deps/onig/enc/utf32_be.c
  deps/onig/enc/utf32_le.c
  deps/onig/enc/utf8.c
)
target_include_directories(oniguruma PRIVATE
  deps/onig
  deps/onig/enc
)
target_compile_definitions(oniguruma PRIVATE
  $<$<PLATFORM_ID:Windows>:ONIG_EXTERN=extern>
)
target_include_directories(oniguruma INTERFACE
  deps/onig
)
