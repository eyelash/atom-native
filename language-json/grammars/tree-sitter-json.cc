#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_json();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_json() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "JSON",
    "source.json",
    tree_sitter_json()
  );

  grammar->setFileTypes(
    "avsc",
    "babelrc",
    "bowerrc",
    "composer.lock",
    "geojson",
    "gltf",
    "htmlhintrc",
    "ipynb",
    "jscsrc",
    "jshintrc",
    "jslintrc",
    "json",
    "jsonl",
    "jsonld",
    "languagebabel",
    "ldj",
    "ldjson",
    "Pipfile.lock",
    "schema",
    "stylintrc",
    "template",
    "tern-config",
    "tern-project",
    "tfstate",
    "tfstate.backup",
    "topojson",
    "webapp",
    "webmanifest"
  );

  grammar->setIncreaseIndentPattern(u"^.*(\\{[^}]*|\\[[^\\]]*)$");
  grammar->setDecreaseIndentPattern(u"^\\s*[}\\]],?\\s*$");

  grammar->setScopes(
    scope("value", "source.json"),

    scope("object", "meta.structure.dictionary.json"),

    scope("string", "string.quoted.double"),

    scope("string_content", array(
      match(u"^http://", "markup.underline.link.http.hyperlink"),
      match(u"^https://", "markup.underline.link.https.hyperlink")
    )),

    scope("pair > string:nth-child(0)", "string.quoted.double.dictionary.key.json"),

    scope("escape_sequence", "constant.character.escape"),

    scope("number", "constant.numeric"),
    scope("true", "constant.language"),
    scope("false", "constant.language"),
    scope("null", "constant.language"),

    scope("\"{\"", "punctuation.definition.dictionary.begin"),
    scope("\"}\"", "punctuation.definition.dictionary.end"),
    scope("\":\"", "punctuation.separator.dictionary.key-value"),
    scope("object > \",\"", "punctuation.separator.dictionary.pair"),
    scope("array > \",\"", "punctuation.separator.array"),
    scope("\"[\"", "punctuation.definition.array.begin"),
    scope("\"]\"", "punctuation.definition.array.end"),
    scope("\"\\\"\"", "punctuation.definition.string.json")
  );

  return grammar;
}
