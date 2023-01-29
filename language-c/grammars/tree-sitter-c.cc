#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_c();

extern "C" TreeSitterGrammar *atom_language_c() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "C",
    "source.c",
    tree_sitter_c()
  );

  grammar->addFileTypes(
    "h",
    "c",
    "h.in"
  );

  grammar->addScopes("translation_unit", "source.c");
  grammar->addScopes("comment", "comment.block");

  grammar->addScopes("identifier",
    TreeSitterGrammar::Match{u"^[A-Z\\d_]+$","constant.other"}
  );

  grammar->addScopes("\"#if\"", "keyword.control.directive");
  grammar->addScopes("\"#ifdef\"", "keyword.control.directive");
  grammar->addScopes("\"#ifndef\"", "keyword.control.directive");
  grammar->addScopes("\"#elif\"", "keyword.control.directive");
  grammar->addScopes("\"#else\"", "keyword.control.directive");
  grammar->addScopes("\"#endif\"", "keyword.control.directive");
  grammar->addScopes("\"#define\"", "keyword.control.directive");
  grammar->addScopes("\"#include\"", "keyword.control.directive");
  grammar->addScopes("preproc_directive", "keyword.control.directive");

  grammar->addScopes("\"if\"", "keyword.control");
  grammar->addScopes("\"else\"", "keyword.control");
  grammar->addScopes("\"do\"", "keyword.control");
  grammar->addScopes("\"for\"", "keyword.control");
  grammar->addScopes("\"while\"", "keyword.control");
  grammar->addScopes("\"break\"", "keyword.control");
  grammar->addScopes("\"continue\"", "keyword.control");
  grammar->addScopes("\"return\"", "keyword.control");
  grammar->addScopes("\"switch\"", "keyword.control");
  grammar->addScopes("\"case\"", "keyword.control");
  grammar->addScopes("\"default\"", "keyword.control");
  grammar->addScopes("\"goto\"", "keyword.control");

  grammar->addScopes("\"struct\"", "keyword.control");
  grammar->addScopes("\"enum\"", "keyword.control");
  grammar->addScopes("\"union\"", "keyword.control");
  grammar->addScopes("\"typedef\"", "keyword.control");

  grammar->addScopes("preproc_function_def > identifier:nth-child(1)", "entity.name.function.preprocessor");
  grammar->addScopes("preproc_arg", "meta.preprocessor.macro");

  grammar->addScopes({
    "call_expression > identifier",
    "call_expression > field_expression > field_identifier",
    "function_declarator > identifier"
  }, "entity.name.function");

  grammar->addScopes("statement_identifier", "constant.variable");

  grammar->addScopes("field_identifier", "variable.other.member");

  grammar->addScopes("type_identifier", "support.storage.type");
  grammar->addScopes("primitive_type", "support.storage.type");
  grammar->addScopes("\"signed\"", "support.storage.type");
  grammar->addScopes("\"unsigned\"", "support.storage.type");
  grammar->addScopes("\"short\"", "support.storage.type");
  grammar->addScopes("\"long\"", "support.storage.type");

  grammar->addScopes("char_literal", "string.quoted.single");
  grammar->addScopes("string_literal", "string.quoted.double");
  grammar->addScopes("system_lib_string", "string.quoted.other");
  grammar->addScopes("escape_sequence", "constant.character.escape");

  grammar->addScopes("number_literal", "constant.numeric.decimal");
  grammar->addScopes("null", "constant.language.null");
  grammar->addScopes("true", "constant.language.boolean");
  grammar->addScopes("false", "constant.language.boolean");

  grammar->addScopes("auto", "storage.modifier");
  grammar->addScopes("\"extern\"", "storage.modifier");
  grammar->addScopes("\"register\"", "storage.modifier");
  grammar->addScopes("\"static\"", "storage.modifier");
  grammar->addScopes("\"inline\"", "storage.modifier");
  grammar->addScopes("\"const\"", "storage.modifier");
  grammar->addScopes("\"volatile\"", "storage.modifier");
  grammar->addScopes("\"restrict\"", "storage.modifier");
  grammar->addScopes("\"_Atomic\"", "storage.modifier");
  grammar->addScopes("function_specifier", "storage.modifier");

  grammar->addScopes("\";\"", "punctuation.terminator.statement");
  grammar->addScopes("\"[\"", "punctuation.definition.begin.bracket.square");
  grammar->addScopes("\"]\"", "punctuation.definition.end.bracket.square");
  grammar->addScopes("\",\"", "punctuation.separator.delimiter");
  grammar->addScopes("char_literal > \"\'\"", "punctuation.definition.string");
  grammar->addScopes("string_literal > \"\\\"\"", "punctuation.definition.string");
  grammar->addScopes("\"{\"", "punctuation.section.block.begin.bracket.curly");
  grammar->addScopes("\"}\"", "punctuation.section.block.end.bracket.curly");
  grammar->addScopes("\"(\"", "punctuation.section.parens.begin.bracket.round");
  grammar->addScopes("\")\"", "punctuation.section.parens.end.bracket.round");

  grammar->addScopes("\"sizeof\"", "keyword.operator.sizeof");
  grammar->addScopes("\".\"", "keyword.operator.member");
  grammar->addScopes("\"->\"", "keyword.operator.member");
  grammar->addScopes("\"*\"", "keyword.operator");
  grammar->addScopes("\"-\"", "keyword.operator");
  grammar->addScopes("\"+\"", "keyword.operator");
  grammar->addScopes("\"/\"", "keyword.operator");
  grammar->addScopes("\"%\"", "keyword.operator");
  grammar->addScopes("\"++\"", "keyword.operator");
  grammar->addScopes("\"--\"", "keyword.operator");
  grammar->addScopes("\"==\"", "keyword.operator");
  grammar->addScopes("\"!\"", "keyword.operator");
  grammar->addScopes("\"!=\"", "keyword.operator");
  grammar->addScopes("\"<\"", "keyword.operator");
  grammar->addScopes("\">\"", "keyword.operator");
  grammar->addScopes("\">=\"", "keyword.operator");
  grammar->addScopes("\"<=\"", "keyword.operator");
  grammar->addScopes("\"&&\"", "keyword.operator");
  grammar->addScopes("\"||\"", "keyword.operator");
  grammar->addScopes("\"&\"", "keyword.operator");
  grammar->addScopes("\"|\"", "keyword.operator");
  grammar->addScopes("\"^\"", "keyword.operator");
  grammar->addScopes("\"~\"", "keyword.operator");
  grammar->addScopes("\"<<\"", "keyword.operator");
  grammar->addScopes("\">>\"", "keyword.operator");
  grammar->addScopes("\"=\"", "keyword.operator");
  grammar->addScopes("\"+=\"", "keyword.operator");
  grammar->addScopes("\"-=\"", "keyword.operator");
  grammar->addScopes("\"*=\"", "keyword.operator");
  grammar->addScopes("\"/=\"", "keyword.operator");
  grammar->addScopes("\"%=\"", "keyword.operator");
  grammar->addScopes("\"<<=\"", "keyword.operator");
  grammar->addScopes("\">>=\"", "keyword.operator");
  grammar->addScopes("\"&=\"", "keyword.operator");
  grammar->addScopes("\"^=\"", "keyword.operator");
  grammar->addScopes("\"|=\"", "keyword.operator");
  grammar->addScopes("\"?\"", "keyword.operator");
  grammar->addScopes("\":\"", "keyword.operator");

  return grammar;
}
