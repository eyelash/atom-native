{
  "targets": [
    {
      "target_name": "tree_sitter_runtime_binding",
      "dependencies": ["tree_sitter"],
      "sources": [
        "src/bindings/binding.cc",
        "src/bindings/conversions.cc",
        "src/bindings/language.cc",
        "src/bindings/logger.cc",
        "src/bindings/node.cc",
        "src/bindings/parser.cc",
        "src/bindings/query.cc",
        "src/bindings/tree.cc",
        "src/bindings/tree_cursor.cc",
        "src/bindings/util.cc",
      ],
      "include_dirs": [
        "vendor/tree-sitter/lib/include",
        "vendor/superstring",
        "<!(node -e \"require('nan')\")",
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'MACOSX_DEPLOYMENT_TARGET': '10.9',
          },
        }]
      ],
      "cflags": [
        "-std=c++0x",
      ],
      'xcode_settings': {
        'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
      },
    },
    {
      "target_name": "tree_sitter",
      'type': 'static_library',
      "sources": [
        "vendor/tree-sitter/lib/src/lib.c"
      ],
      "include_dirs": [
        "vendor/tree-sitter/lib/src",
        "vendor/tree-sitter/lib/include",
      ],
      "cflags": [
        "-std=c99"
      ]
    }
  ],
}
