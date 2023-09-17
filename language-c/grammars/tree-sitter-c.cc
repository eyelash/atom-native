#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_c();

using namespace TreeSitterGrammarDSL;

static TSNode ts_node_last_named_child(TSNode node) {
  return ts_node_named_child(node, ts_node_named_child_count(node) - 1);
}

extern "C" TreeSitterGrammar *atom_language_c() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "C",
    "source.c",
    tree_sitter_c()
  );

  grammar->setInjectionRegex(u"c|C");

  grammar->setFileTypes(
    "h",
    "c",
    "h.in"
  );

  for (const char *nodeType : {"preproc_def", "preproc_function_def"}) {
    grammar->addInjectionPoint({
      nodeType,
      [](TSNode) -> std::u16string { return u"c"; },
      [](TSNode node) -> std::vector<TSNode> { return {ts_node_last_named_child(node)}; }
    });
  }

  grammar->setIncreaseIndentPattern(uR"""((?x)
     ^ .* \{ [^}"']* $
    |^ .* \( [^)"']* $
    |^ \s* (public|private|protected): \s* $
    |^ \s* @(public|private|protected) \s* $
    |^ \s* \{ \} $
  )""");
  grammar->setDecreaseIndentPattern(uR"""((?x)
     ^ \s* (\s* /[*] .* [*]/ \s*)* \}
    |^ \s* (\s* /[*] .* [*]/ \s*)* \)
    |^ \s* (public|private|protected): \s* $
    |^ \s* @(public|private|protected) \s* $
  )""");

  grammar->setScopes(
    scope("translation_unit", "source.c"),
    scope("comment", "comment.block"),

    scope("identifier",
      match(u"^[A-Z\\d_]+$", "constant.other")
    ),

    scope("\"#if\"", "keyword.control.directive"),
    scope("\"#ifdef\"", "keyword.control.directive"),
    scope("\"#ifndef\"", "keyword.control.directive"),
    scope("\"#elif\"", "keyword.control.directive"),
    scope("\"#else\"", "keyword.control.directive"),
    scope("\"#endif\"", "keyword.control.directive"),
    scope("\"#define\"", "keyword.control.directive"),
    scope("\"#include\"", "keyword.control.directive"),
    scope("preproc_directive", "keyword.control.directive"),

    scope("\"if\"", "keyword.control"),
    scope("\"else\"", "keyword.control"),
    scope("\"do\"", "keyword.control"),
    scope("\"for\"", "keyword.control"),
    scope("\"while\"", "keyword.control"),
    scope("\"break\"", "keyword.control"),
    scope("\"continue\"", "keyword.control"),
    scope("\"return\"", "keyword.control"),
    scope("\"switch\"", "keyword.control"),
    scope("\"case\"", "keyword.control"),
    scope("\"default\"", "keyword.control"),
    scope("\"goto\"", "keyword.control"),

    scope("\"struct\"", "keyword.control"),
    scope("\"enum\"", "keyword.control"),
    scope("\"union\"", "keyword.control"),
    scope("\"typedef\"", "keyword.control"),

    scope("preproc_function_def > identifier:nth-child(1)", "entity.name.function.preprocessor"),
    scope("preproc_arg", "meta.preprocessor.macro"),

    scope(array(
      "call_expression > identifier",
      "call_expression > field_expression > field_identifier",
      "function_declarator > identifier"
    ), "entity.name.function"),

    scope("statement_identifier", "constant.variable"),

    scope("field_identifier", "variable.other.member"),

    scope("type_identifier", "support.storage.type"),
    scope("primitive_type", "support.storage.type"),
    scope("\"signed\"", "support.storage.type"),
    scope("\"unsigned\"", "support.storage.type"),
    scope("\"short\"", "support.storage.type"),
    scope("\"long\"", "support.storage.type"),

    scope("char_literal", "string.quoted.single"),
    scope("string_literal", "string.quoted.double"),
    scope("system_lib_string", "string.quoted.other"),
    scope("escape_sequence", "constant.character.escape"),

    scope("number_literal", "constant.numeric.decimal"),
    scope("null", "constant.language.null"),
    scope("true", "constant.language.boolean"),
    scope("false", "constant.language.boolean"),

    scope("auto", "storage.modifier"),
    scope("\"extern\"", "storage.modifier"),
    scope("\"register\"", "storage.modifier"),
    scope("\"static\"", "storage.modifier"),
    scope("\"inline\"", "storage.modifier"),
    scope("\"const\"", "storage.modifier"),
    scope("\"volatile\"", "storage.modifier"),
    scope("\"restrict\"", "storage.modifier"),
    scope("\"_Atomic\"", "storage.modifier"),
    scope("function_specifier", "storage.modifier"),

    scope("\";\"", "punctuation.terminator.statement"),
    scope("\"[\"", "punctuation.definition.begin.bracket.square"),
    scope("\"]\"", "punctuation.definition.end.bracket.square"),
    scope("\",\"", "punctuation.separator.delimiter"),
    scope("char_literal > \"\'\"", "punctuation.definition.string"),
    scope("string_literal > \"\\\"\"", "punctuation.definition.string"),
    scope("\"{\"", "punctuation.section.block.begin.bracket.curly"),
    scope("\"}\"", "punctuation.section.block.end.bracket.curly"),
    scope("\"(\"", "punctuation.section.parens.begin.bracket.round"),
    scope("\")\"", "punctuation.section.parens.end.bracket.round"),

    scope("\"sizeof\"", "keyword.operator.sizeof"),
    scope("\".\"", "keyword.operator.member"),
    scope("\"->\"", "keyword.operator.member"),
    scope("\"*\"", "keyword.operator"),
    scope("\"-\"", "keyword.operator"),
    scope("\"+\"", "keyword.operator"),
    scope("\"/\"", "keyword.operator"),
    scope("\"%\"", "keyword.operator"),
    scope("\"++\"", "keyword.operator"),
    scope("\"--\"", "keyword.operator"),
    scope("\"==\"", "keyword.operator"),
    scope("\"!\"", "keyword.operator"),
    scope("\"!=\"", "keyword.operator"),
    scope("\"<\"", "keyword.operator"),
    scope("\">\"", "keyword.operator"),
    scope("\">=\"", "keyword.operator"),
    scope("\"<=\"", "keyword.operator"),
    scope("\"&&\"", "keyword.operator"),
    scope("\"||\"", "keyword.operator"),
    scope("\"&\"", "keyword.operator"),
    scope("\"|\"", "keyword.operator"),
    scope("\"^\"", "keyword.operator"),
    scope("\"~\"", "keyword.operator"),
    scope("\"<<\"", "keyword.operator"),
    scope("\">>\"", "keyword.operator"),
    scope("\"=\"", "keyword.operator"),
    scope("\"+=\"", "keyword.operator"),
    scope("\"-=\"", "keyword.operator"),
    scope("\"*=\"", "keyword.operator"),
    scope("\"/=\"", "keyword.operator"),
    scope("\"%=\"", "keyword.operator"),
    scope("\"<<=\"", "keyword.operator"),
    scope("\">>=\"", "keyword.operator"),
    scope("\"&=\"", "keyword.operator"),
    scope("\"^=\"", "keyword.operator"),
    scope("\"|=\"", "keyword.operator"),
    scope("\"?\"", "keyword.operator"),
    scope("\":\"", "keyword.operator")
  );

  return grammar;
}
