#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_c();

extern "C" TreeSitterGrammar *atom_language_c() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(tree_sitter_c());

  grammar->addScope("translation_unit", "source.c");
  grammar->addScope("comment", "comment.block");

  /*'identifier': [
    {match: '^[A-Z\\d_]+$', scopes: 'constant.other'}
  ]*/

  grammar->addScope("\"#if\"", "keyword.control.directive");
  grammar->addScope("\"#ifdef\"", "keyword.control.directive");
  grammar->addScope("\"#ifndef\"", "keyword.control.directive");
  grammar->addScope("\"#elif\"", "keyword.control.directive");
  grammar->addScope("\"#else\"", "keyword.control.directive");
  grammar->addScope("\"#endif\"", "keyword.control.directive");
  grammar->addScope("\"#define\"", "keyword.control.directive");
  grammar->addScope("\"#include\"", "keyword.control.directive");
  grammar->addScope("preproc_directive", "keyword.control.directive");

  grammar->addScope("\"if\"", "keyword.control");
  grammar->addScope("\"else\"", "keyword.control");
  grammar->addScope("\"do\"", "keyword.control");
  grammar->addScope("\"for\"", "keyword.control");
  grammar->addScope("\"while\"", "keyword.control");
  grammar->addScope("\"break\"", "keyword.control");
  grammar->addScope("\"continue\"", "keyword.control");
  grammar->addScope("\"return\"", "keyword.control");
  grammar->addScope("\"switch\"", "keyword.control");
  grammar->addScope("\"case\"", "keyword.control");
  grammar->addScope("\"default\"", "keyword.control");
  grammar->addScope("\"goto\"", "keyword.control");

  grammar->addScope("\"struct\"", "keyword.control");
  grammar->addScope("\"enum\"", "keyword.control");
  grammar->addScope("\"union\"", "keyword.control");
  grammar->addScope("\"typedef\"", "keyword.control");

  grammar->addScope("preproc_function_def > identifier:nth-child(1)", "entity.name.function.preprocessor");
  grammar->addScope("preproc_arg", "meta.preprocessor.macro");

  grammar->addScope(R"(
    call_expression > identifier,
    call_expression > field_expression > field_identifier,
    function_declarator > identifier
  )", "entity.name.function");

  grammar->addScope("statement_identifier", "constant.variable");

  grammar->addScope("field_identifier", "variable.other.member");

  grammar->addScope("type_identifier", "support.storage.type");
  grammar->addScope("primitive_type", "support.storage.type");
  grammar->addScope("\"signed\"", "support.storage.type");
  grammar->addScope("\"unsigned\"", "support.storage.type");
  grammar->addScope("\"short\"", "support.storage.type");
  grammar->addScope("\"long\"", "support.storage.type");

  grammar->addScope("char_literal", "string.quoted.single");
  grammar->addScope("string_literal", "string.quoted.double");
  grammar->addScope("system_lib_string", "string.quoted.other");
  grammar->addScope("escape_sequence", "constant.character.escape");

  grammar->addScope("number_literal", "constant.numeric.decimal");
  grammar->addScope("null", "constant.language.null");
  grammar->addScope("true", "constant.language.boolean");
  grammar->addScope("false", "constant.language.boolean");

  grammar->addScope("auto", "storage.modifier");
  grammar->addScope("\"extern\"", "storage.modifier");
  grammar->addScope("\"register\"", "storage.modifier");
  grammar->addScope("\"static\"", "storage.modifier");
  grammar->addScope("\"inline\"", "storage.modifier");
  grammar->addScope("\"const\"", "storage.modifier");
  grammar->addScope("\"volatile\"", "storage.modifier");
  grammar->addScope("\"restrict\"", "storage.modifier");
  grammar->addScope("\"_Atomic\"", "storage.modifier");
  grammar->addScope("function_specifier", "storage.modifier");

  grammar->addScope("\";\"", "punctuation.terminator.statement");
  grammar->addScope("\"[\"", "punctuation.definition.begin.bracket.square");
  grammar->addScope("\"]\"", "punctuation.definition.end.bracket.square");
  grammar->addScope("\",\"", "punctuation.separator.delimiter");
  grammar->addScope("char_literal > \"\'\"", "punctuation.definition.string");
  grammar->addScope("string_literal > \"\\\"\"", "punctuation.definition.string");
  grammar->addScope("\"{\"", "punctuation.section.block.begin.bracket.curly");
  grammar->addScope("\"}\"", "punctuation.section.block.end.bracket.curly");
  grammar->addScope("\"(\"", "punctuation.section.parens.begin.bracket.round");
  grammar->addScope("\")\"", "punctuation.section.parens.end.bracket.round");

  grammar->addScope("\"sizeof\"", "keyword.operator.sizeof");
  grammar->addScope("\".\"", "keyword.operator.member");
  grammar->addScope("\"->\"", "keyword.operator.member");
  grammar->addScope("\"*\"", "keyword.operator");
  grammar->addScope("\"-\"", "keyword.operator");
  grammar->addScope("\"+\"", "keyword.operator");
  grammar->addScope("\"/\"", "keyword.operator");
  grammar->addScope("\"%\"", "keyword.operator");
  grammar->addScope("\"++\"", "keyword.operator");
  grammar->addScope("\"--\"", "keyword.operator");
  grammar->addScope("\"==\"", "keyword.operator");
  grammar->addScope("\"!\"", "keyword.operator");
  grammar->addScope("\"!=\"", "keyword.operator");
  grammar->addScope("\"<\"", "keyword.operator");
  grammar->addScope("\">\"", "keyword.operator");
  grammar->addScope("\">=\"", "keyword.operator");
  grammar->addScope("\"<=\"", "keyword.operator");
  grammar->addScope("\"&&\"", "keyword.operator");
  grammar->addScope("\"||\"", "keyword.operator");
  grammar->addScope("\"&\"", "keyword.operator");
  grammar->addScope("\"|\"", "keyword.operator");
  grammar->addScope("\"^\"", "keyword.operator");
  grammar->addScope("\"~\"", "keyword.operator");
  grammar->addScope("\"<<\"", "keyword.operator");
  grammar->addScope("\">>\"", "keyword.operator");
  grammar->addScope("\"=\"", "keyword.operator");
  grammar->addScope("\"+=\"", "keyword.operator");
  grammar->addScope("\"-=\"", "keyword.operator");
  grammar->addScope("\"*=\"", "keyword.operator");
  grammar->addScope("\"/=\"", "keyword.operator");
  grammar->addScope("\"%=\"", "keyword.operator");
  grammar->addScope("\"<<=\"", "keyword.operator");
  grammar->addScope("\">>=\"", "keyword.operator");
  grammar->addScope("\"&=\"", "keyword.operator");
  grammar->addScope("\"^=\"", "keyword.operator");
  grammar->addScope("\"|=\"", "keyword.operator");
  grammar->addScope("\"?\"", "keyword.operator");
  grammar->addScope("\":\"", "keyword.operator");

  return grammar;
}
