{
    "targets": [
        {
            "target_name": "superstring",
            "dependencies": [
                "superstring_core"
            ],
            "sources": [
                "src/bindings/bindings.cc",
                "src/bindings/marker-index-wrapper.cc",
                "src/bindings/patch-wrapper.cc",
                "src/bindings/point-wrapper.cc",
                "src/bindings/range-wrapper.cc",
                "src/bindings/text-buffer-wrapper.cc",
                "src/bindings/text-buffer-snapshot-wrapper.cc",
                "src/bindings/text-reader.cc",
                "src/bindings/string-conversion.cc",
                "src/bindings/text-writer.cc",
            ],
            "include_dirs": [
              "src",
              "<!(node -e \"require('nan')\")"
            ],
        },
        {
            "target_name": "superstring_core",
            "type": "static_library",
            "dependencies": [
                "./vendor/pcre/pcre.gyp:pcre",
            ],
            "sources": [
                "src/encoding-conversion.cc",
                "src/marker-index.cc",
                "src/patch.cc",
                "src/point.cc",
                "src/range.cc",
                "src/regex.cc",
                "src/text.cc",
                "src/text-buffer.cc",
                "src/text-slice.cc",
                "src/text-diff.cc",
                "src/libmba-diff.cc",
            ],
            "include_dirs": [
                "vendor/libcxx"
            ],
            "conditions": [
                ['OS=="mac"', {
                    'link_settings': {
                        'libraries': ['libiconv.dylib'],
                    }
                }],
                ['OS=="win"', {
                   'sources': [
                       'vendor/win-iconv/win_iconv.c',
                    ],
                    'include_dirs': [
                        'vendor/win-iconv'
                    ],
                    'defines': [
                        'WINICONV_CONST=',
                        'PCRE2_STATIC',
                    ]
                }],
            ],
        }
    ],

    "variables": {
        "tests": 0
    },

    "conditions": [
        # If --tests is passed to node-gyp configure, we'll build a standalone
        # executable that runs tests on the patch.
        ['tests != 0', {
            "targets": [{
                "target_name": "tests",
                "type": "executable",
                "cflags_cc!": ["-fno-exceptions"],
                "defines": [
                    "CATCH_CONFIG_CPP11_NO_IS_ENUM"
                ],
                "sources": [
                    "test/native/test-helpers.cc",
                    "test/native/tests.cc",
                    "test/native/encoding-conversion-test.cc",
                    "test/native/patch-test.cc",
                    "test/native/text-buffer-test.cc",
                    "test/native/text-test.cc",
                    "test/native/text-diff-test.cc",
                ],
                "include_dirs": [
                    "vendor",
                    "src",
                ],
                "dependencies": [
                    "superstring_core"
                ],
                "conditions": [
                    ['OS=="mac"', {
                        'cflags': [
                            '-mmacosx-version-min=10.8'
                        ],
                        "xcode_settings": {
                            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                            'MACOSX_DEPLOYMENT_TARGET': '10.8',
                        }
                    }]
                ]
            }]
        }]
    ],

    "target_defaults": {
        "cflags_cc": ["-std=c++11"],
        "conditions": [
            ['OS=="mac"', {
                "xcode_settings": {
                    'CLANG_CXX_LIBRARY': 'libc++',
                    'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
                }
            }],
            ['OS=="win"', {
                "link_settings": {
                    "libraries": ["ws2_32.lib"]
                },
                "defines": [
                    "NOMINMAX"
                ],
            }]
        ]
    }
}
