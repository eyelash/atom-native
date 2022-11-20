cmake_minimum_required(VERSION 3.13)
project(git-utils)

add_library(libgit2 STATIC)
target_link_libraries(libgit2
  zlib
  http_parser
  $<$<PLATFORM_ID:Windows>:pcre>
)
target_sources(libgit2 PRIVATE
  deps/libgit2/src/annotated_commit.c
# deps/libgit2/src/annotated_commit.h
  deps/libgit2/src/apply.c
# deps/libgit2/src/apply.h
  deps/libgit2/src/alloc.c
# deps/libgit2/src/alloc.h
# deps/libgit2/src/array.h
  deps/libgit2/src/attr.c
# deps/libgit2/src/attr.h
  deps/libgit2/src/attr_file.c
# deps/libgit2/src/attr_file.h
  deps/libgit2/src/attrcache.c
# deps/libgit2/src/attrcache.h
# deps/libgit2/src/bitvec.h
  deps/libgit2/src/blame.c
# deps/libgit2/src/blame.h
  deps/libgit2/src/blame_git.c
# deps/libgit2/src/blame_git.h
  deps/libgit2/src/blob.c
# deps/libgit2/src/blob.h
  deps/libgit2/src/branch.c
# deps/libgit2/src/branch.h
  deps/libgit2/src/buf_text.c
# deps/libgit2/src/buf_text.h
  deps/libgit2/src/buffer.c
# deps/libgit2/src/buffer.h
  deps/libgit2/src/cache.c
# deps/libgit2/src/cache.h
# deps/libgit2/src/cc-compat.h
  deps/libgit2/src/checkout.c
# deps/libgit2/src/checkout.h
  deps/libgit2/src/cherrypick.c
  deps/libgit2/src/clone.c
# deps/libgit2/src/clone.h
  deps/libgit2/src/commit.c
# deps/libgit2/src/commit.h
  deps/libgit2/src/commit_list.c
# deps/libgit2/src/commit_list.h
# deps/libgit2/src/common.h
  deps/libgit2/src/config.c
# deps/libgit2/src/config.h
  deps/libgit2/src/config_cache.c
  deps/libgit2/src/config_entries.c
# deps/libgit2/src/config_entries.h
  deps/libgit2/src/config_file.c
# deps/libgit2/src/config_file.h
  deps/libgit2/src/config_parse.c
# deps/libgit2/src/config_parse.h
  deps/libgit2/src/crlf.c
  deps/libgit2/src/date.c
  deps/libgit2/src/delta.c
# deps/libgit2/src/delta.h
  deps/libgit2/src/describe.c
  deps/libgit2/src/diff.c
# deps/libgit2/src/diff.h
  deps/libgit2/src/diff_driver.c
# deps/libgit2/src/diff_driver.h
  deps/libgit2/src/diff_file.c
# deps/libgit2/src/diff_file.h
  deps/libgit2/src/diff_generate.c
# deps/libgit2/src/diff_generate.h
  deps/libgit2/src/diff_parse.c
# deps/libgit2/src/diff_parse.h
  deps/libgit2/src/diff_print.c
  deps/libgit2/src/diff_stats.c
  deps/libgit2/src/diff_tform.c
# deps/libgit2/src/diff_tform.h
  deps/libgit2/src/diff_xdiff.c
# deps/libgit2/src/diff_xdiff.h
  deps/libgit2/src/errors.c
  deps/libgit2/src/fetch.c
# deps/libgit2/src/fetch.h
  deps/libgit2/src/fetchhead.c
# deps/libgit2/src/fetchhead.h
  deps/libgit2/src/filebuf.c
# deps/libgit2/src/filebuf.h
  deps/libgit2/src/fileops.c
# deps/libgit2/src/fileops.h
  deps/libgit2/src/filter.c
# deps/libgit2/src/filter.h
  deps/libgit2/src/wildmatch.c
# deps/libgit2/src/wildmatch.h
  deps/libgit2/src/global.c
# deps/libgit2/src/global.h
  deps/libgit2/src/graph.c
  deps/libgit2/src/hash.c
# deps/libgit2/src/hash.h
  deps/libgit2/src/hashsig.c
  deps/libgit2/src/ident.c
  deps/libgit2/src/idxmap.c
# deps/libgit2/src/idxmap.h
  deps/libgit2/src/ignore.c
# deps/libgit2/src/ignore.h
  deps/libgit2/src/index.c
# deps/libgit2/src/index.h
  deps/libgit2/src/indexer.c
# deps/libgit2/src/indexer.h
# deps/libgit2/src/integer.h
  deps/libgit2/src/iterator.c
# deps/libgit2/src/iterator.h
# deps/libgit2/src/khash.h
  deps/libgit2/src/mailmap.c
# deps/libgit2/src/mailmap.h
# deps/libgit2/src/map.h
  deps/libgit2/src/merge.c
# deps/libgit2/src/merge.h
  deps/libgit2/src/merge_driver.c
# deps/libgit2/src/merge_driver.h
  deps/libgit2/src/merge_file.c
  deps/libgit2/src/message.c
# deps/libgit2/src/message.h
  deps/libgit2/src/mwindow.c
# deps/libgit2/src/mwindow.h
  deps/libgit2/src/netops.c
# deps/libgit2/src/netops.h
  deps/libgit2/src/notes.c
# deps/libgit2/src/notes.h
  deps/libgit2/src/object.c
# deps/libgit2/src/object.h
  deps/libgit2/src/object_api.c
  deps/libgit2/src/odb.c
# deps/libgit2/src/odb.h
  deps/libgit2/src/odb_loose.c
  deps/libgit2/src/odb_mempack.c
  deps/libgit2/src/odb_pack.c
  deps/libgit2/src/offmap.c
# deps/libgit2/src/offmap.h
  deps/libgit2/src/oid.c
# deps/libgit2/src/oid.h
  deps/libgit2/src/oidarray.c
# deps/libgit2/src/oidarray.h
  deps/libgit2/src/oidmap.c
# deps/libgit2/src/oidmap.h
  deps/libgit2/src/pack-objects.c
# deps/libgit2/src/pack-objects.h
  deps/libgit2/src/pack.c
# deps/libgit2/src/pack.h
  deps/libgit2/src/parse.c
# deps/libgit2/src/parse.h
  deps/libgit2/src/patch.c
# deps/libgit2/src/patch.h
  deps/libgit2/src/patch_generate.c
# deps/libgit2/src/patch_generate.h
  deps/libgit2/src/patch_parse.c
# deps/libgit2/src/patch_parse.h
  deps/libgit2/src/path.c
# deps/libgit2/src/path.h
  deps/libgit2/src/pathspec.c
# deps/libgit2/src/pathspec.h
  deps/libgit2/src/pool.c
# deps/libgit2/src/pool.h
  deps/libgit2/src/posix.c
# deps/libgit2/src/posix.h
  deps/libgit2/src/pqueue.c
# deps/libgit2/src/pqueue.h
  deps/libgit2/src/proxy.c
# deps/libgit2/src/proxy.h
  deps/libgit2/src/push.c
# deps/libgit2/src/push.h
  deps/libgit2/src/rebase.c
  deps/libgit2/src/refdb.c
# deps/libgit2/src/refdb.h
  deps/libgit2/src/refdb_fs.c
# deps/libgit2/src/refdb_fs.h
  deps/libgit2/src/reflog.c
# deps/libgit2/src/reflog.h
  deps/libgit2/src/refs.c
# deps/libgit2/src/refs.h
  deps/libgit2/src/refspec.c
# deps/libgit2/src/refspec.h
  deps/libgit2/src/remote.c
# deps/libgit2/src/remote.h
# deps/libgit2/src/repo_template.h
  deps/libgit2/src/repository.c
# deps/libgit2/src/repository.h
  deps/libgit2/src/reset.c
  deps/libgit2/src/revert.c
  deps/libgit2/src/revparse.c
  deps/libgit2/src/revwalk.c
# deps/libgit2/src/revwalk.h
  deps/libgit2/src/settings.c
  deps/libgit2/src/sha1_lookup.c
# deps/libgit2/src/sha1_lookup.h
  deps/libgit2/src/signature.c
# deps/libgit2/src/signature.h
  deps/libgit2/src/sortedcache.c
# deps/libgit2/src/sortedcache.h
  deps/libgit2/src/stash.c
  deps/libgit2/src/status.c
# deps/libgit2/src/status.h
  deps/libgit2/src/allocators/stdalloc.c
# deps/libgit2/src/allocators/stdalloc.h
# deps/libgit2/src/stream.h
  deps/libgit2/src/strmap.c
# deps/libgit2/src/strmap.h
# deps/libgit2/src/strnlen.h
  deps/libgit2/src/submodule.c
# deps/libgit2/src/submodule.h
  deps/libgit2/src/sysdir.c
# deps/libgit2/src/sysdir.h
  deps/libgit2/src/tag.c
# deps/libgit2/src/tag.h
  deps/libgit2/src/thread-utils.c
# deps/libgit2/src/thread-utils.h
  deps/libgit2/src/trace.c
# deps/libgit2/src/trace.h
  deps/libgit2/src/trailer.c
  deps/libgit2/src/transaction.c
# deps/libgit2/src/transaction.h
  deps/libgit2/src/transport.c
  deps/libgit2/src/tree-cache.c
# deps/libgit2/src/tree-cache.h
  deps/libgit2/src/tree.c
# deps/libgit2/src/tree.h
  deps/libgit2/src/tsort.c
# deps/libgit2/src/userdiff.h
  deps/libgit2/src/util.c
# deps/libgit2/src/util.h
  deps/libgit2/src/varint.c
# deps/libgit2/src/varint.h
  deps/libgit2/src/vector.c
# deps/libgit2/src/vector.h
  deps/libgit2/src/worktree.c
# deps/libgit2/src/worktree.h
  deps/libgit2/src/zstream.c
# deps/libgit2/src/zstream.h
  deps/libgit2/src/streams/registry.c
# deps/libgit2/src/streams/registry.h
  deps/libgit2/src/streams/mbedtls.c
# deps/libgit2/src/streams/mbedtls.h
  deps/libgit2/src/streams/openssl.c
# deps/libgit2/src/streams/openssl.h
  deps/libgit2/src/streams/socket.c
# deps/libgit2/src/streams/socket.h
  deps/libgit2/src/streams/stransport.c
# deps/libgit2/src/streams/stransport.h
  deps/libgit2/src/streams/tls.c
# deps/libgit2/src/streams/tls.h
  deps/libgit2/src/transports/auth.c
# deps/libgit2/src/transports/auth.h
  deps/libgit2/src/transports/auth_negotiate.c
# deps/libgit2/src/transports/auth_negotiate.h
  deps/libgit2/src/transports/cred.c
# deps/libgit2/src/transports/cred.h
  deps/libgit2/src/transports/cred_helpers.c
  deps/libgit2/src/transports/git.c
  deps/libgit2/src/transports/http.c
# deps/libgit2/src/transports/http.h
  deps/libgit2/src/transports/local.c
  deps/libgit2/src/transports/smart.c
# deps/libgit2/src/transports/smart.h
  deps/libgit2/src/transports/smart_pkt.c
  deps/libgit2/src/transports/smart_protocol.c
  deps/libgit2/src/transports/ssh.c
# deps/libgit2/src/transports/ssh.h
  deps/libgit2/src/transports/winhttp.c
# deps/libgit2/src/xdiff/xdiff.h
  deps/libgit2/src/xdiff/xdiffi.c
# deps/libgit2/src/xdiff/xdiffi.h
  deps/libgit2/src/xdiff/xemit.c
# deps/libgit2/src/xdiff/xemit.h
  deps/libgit2/src/xdiff/xhistogram.c
# deps/libgit2/src/xdiff/xinclude.h
# deps/libgit2/src/xdiff/xmacros.h
  deps/libgit2/src/xdiff/xmerge.c
  deps/libgit2/src/xdiff/xpatience.c
  deps/libgit2/src/xdiff/xprepare.c
# deps/libgit2/src/xdiff/xprepare.h
# deps/libgit2/src/xdiff/xtypes.h
  deps/libgit2/src/xdiff/xutils.c
# deps/libgit2/src/xdiff/xutils.h
  deps/libgit2/src/hash/hash_generic.c
# deps/libgit2/src/hash/hash_generic.h
)
target_include_directories(libgit2 PRIVATE
  deps/libgit2/include
  deps/libgit2/src
)
target_include_directories(libgit2 INTERFACE
  deps/libgit2/include
)
target_compile_definitions(libgit2 PRIVATE
  GIT_THREADS
  LIBGIT2_NO_FEATURES_H
  SRC_UTIL_H_
  $<$<PLATFORM_ID:Linux>:GIT_USE_STAT_MTIM>
  $<$<PLATFORM_ID:Darwin>:GIT_USE_STAT_MTIMESPEC>
  $<$<PLATFORM_ID:Windows>:GIT_WINHTTP>
  $<$<PLATFORM_ID:Windows>:GIT_REGEX_BUILTIN>
)

add_library(zlib STATIC)
target_sources(zlib PRIVATE
  deps/libgit2/deps/zlib/adler32.c
  deps/libgit2/deps/zlib/crc32.c
# deps/libgit2/deps/zlib/crc32.h
  deps/libgit2/deps/zlib/deflate.c
# deps/libgit2/deps/zlib/deflate.h
# deps/libgit2/deps/zlib/gzguts.h
  deps/libgit2/deps/zlib/infback.c
  deps/libgit2/deps/zlib/inffast.c
# deps/libgit2/deps/zlib/inffast.h
# deps/libgit2/deps/zlib/inffixed.h
  deps/libgit2/deps/zlib/inflate.c
# deps/libgit2/deps/zlib/inflate.h
  deps/libgit2/deps/zlib/inftrees.c
# deps/libgit2/deps/zlib/inftrees.h
  deps/libgit2/deps/zlib/trees.c
# deps/libgit2/deps/zlib/trees.h
# deps/libgit2/deps/zlib/zconf.h
# deps/libgit2/deps/zlib/zlib.h
  deps/libgit2/deps/zlib/zutil.c
# deps/libgit2/deps/zlib/zutil.h
)
target_include_directories(zlib PRIVATE
  deps/libgit2/include
  $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/deps/libgit2/deps/regex>
)
target_include_directories(zlib INTERFACE
  deps/libgit2/deps/zlib
)
target_compile_definitions(zlib PRIVATE
  NO_VIZ
  STDC
  NO_GZIP
)

add_library(http_parser STATIC)
target_sources(http_parser PRIVATE
  deps/libgit2/deps/http-parser/http_parser.c
# deps/libgit2/deps/http-parser/http_parser.h
)
target_include_directories(http_parser INTERFACE
  deps/libgit2/deps/http-parser
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_library(pcre STATIC)
  target_sources(pcre PRIVATE
    deps/libgit2/deps/pcre/pcre_byte_order.c
    deps/libgit2/deps/pcre/pcre_chartables.c
    deps/libgit2/deps/pcre/pcre_compile.c
    deps/libgit2/deps/pcre/pcre_config.c
    deps/libgit2/deps/pcre/pcre_dfa_exec.c
    deps/libgit2/deps/pcre/pcre_exec.c
    deps/libgit2/deps/pcre/pcre_fullinfo.c
    deps/libgit2/deps/pcre/pcre_get.c
    deps/libgit2/deps/pcre/pcre_globals.c
    deps/libgit2/deps/pcre/pcre_jit_compile.c
    deps/libgit2/deps/pcre/pcre_maketables.c
    deps/libgit2/deps/pcre/pcre_newline.c
    deps/libgit2/deps/pcre/pcre_ord2utf8.c
    deps/libgit2/deps/pcre/pcre_refcount.c
    deps/libgit2/deps/pcre/pcre_string_utils.c
    deps/libgit2/deps/pcre/pcre_study.c
    deps/libgit2/deps/pcre/pcre_tables.c
    deps/libgit2/deps/pcre/pcre_ucd.c
    deps/libgit2/deps/pcre/pcre_valid_utf8.c
    deps/libgit2/deps/pcre/pcre_version.c
    deps/libgit2/deps/pcre/pcre_xclass.c
    deps/libgit2/deps/pcre/pcreposix.c
  )
  target_compile_definitions(pcre PRIVATE
    SUPPORT_PCRE8=1
    LINK_SIZE=2
    PARENS_NEST_LIMIT=250
    MATCH_LIMIT=10000000
    MATCH_LIMIT_RECURSION="MATCH_LIMIT"
    NEWLINE="LF"
    NO_RECURSE=1
    POSIX_MALLOC_THRESHOLD=10
    BSR_ANYCRLF=0
    MAX_NAME_SIZE=32
    MAX_NAME_COUNT=10000
  )
  target_include_directories(pcre INTERFACE
    deps/libgit2/deps/pcre
  )
endif()
