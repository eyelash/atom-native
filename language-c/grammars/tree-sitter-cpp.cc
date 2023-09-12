#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_cpp();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_cpp() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "C++",
    "source.cpp",
    tree_sitter_cpp()
  );

  grammar->setFileTypes(
    "cc",
    "cpp",
    "cp",
    "cxx",
    "c++",
    "cu",
    "cuh",
    "h",
    "hh",
    "hpp",
    "hxx",
    "h++",
    "inl",
    "ino",
    "ipp",
    "tcc",
    "tpp"
  );

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
    scope("translation_unit", "source.cpp"),
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
    scope("\"class\"", "keyword.control"),
    scope("\"using\"", "keyword.control"),
    scope("\"namespace\"", "keyword.control"),
    scope("\"template\"", "keyword.control"),
    scope("\"typename\"", "keyword.control"),
    scope("\"try\"", "keyword.control"),
    scope("\"catch\"", "keyword.control"),
    scope("\"throw\"", "keyword.control"),
    scope("\"__attribute__\"", "keyword.attribute"),

    scope("preproc_function_def > identifier:nth-child(1)", "entity.name.function.preprocessor"),
    scope("preproc_arg", "meta.preprocessor.macro"),
    scope("preproc_directive", "keyword.control.directive"),

    scope("template_function > identifier",
      match(
        u"^(static|const|dynamic|reinterpret)_cast$",
        "keyword.operator"
      )
    ),

    scope(array(
      "call_expression > identifier",
      "call_expression > field_expression > field_identifier",
      "call_expression > scoped_identifier > identifier",
      "template_function > identifier",
      "template_function > scoped_identifier > identifier",
      "template_method > field_identifier",
      "function_declarator > identifier",
      "function_declarator > field_identifier",
      "function_declarator > scoped_identifier > identifier",
      "destructor_name > identifier"
    ), "entity.name.function"),

    scope("statement_identifier", "constant.variable"),

    scope("field_identifier", "variable.other.member"),

    scope("type_identifier", "support.storage.type"),
    scope("primitive_type", "support.storage.type"),
    scope("\"unsigned\"", "support.storage.type"),
    scope("\"signed\"", "support.storage.type"),
    scope("\"short\"", "support.storage.type"),
    scope("\"long\"", "support.storage.type"),
    scope("auto", "support.storage.type"),

    scope("char_literal", "string.quoted.single"),
    scope("string_literal", "string.quoted.double"),
    scope("system_lib_string", "string.quoted.other"),
    scope("raw_string_literal", "string.quoted.other"),
    scope("escape_sequence", "constant.character.escape"),
    scope("preproc_include > string_literal > escape_sequence", "string.quoted.double"),

    scope("number_literal", "constant.numeric.decimal"),
    scope("null", "constant.language.null"),
    scope("nullptr", "constant.language.null"),
    scope("true", "constant.language.boolean"),
    scope("false", "constant.language.boolean"),

    scope("\"extern\"", "storage.modifier"),
    scope("\"static\"", "storage.modifier"),
    scope("\"register\"", "storage.modifier"),
    scope("\"friend\"", "storage.modifier"),
    scope("\"inline\"", "storage.modifier"),
    scope("\"explicit\"", "storage.modifier"),
    scope("\"const\"", "storage.modifier"),
    scope("\"constexpr\"", "storage.modifier"),
    scope("\"volatile\"", "storage.modifier"),
    scope("\"restrict\"", "storage.modifier"),
    scope("function_specifier", "storage.modifier"),
    scope("\"public\"", "storage.modifier"),
    scope("\"private\"", "storage.modifier"),
    scope("\"protected\"", "storage.modifier"),
    scope("\"final\"", "storage.modifier"),
    scope("\"override\"", "storage.modifier"),
    scope("\"virtual\"", "storage.modifier"),
    scope("\"noexcept\"", "storage.modifier"),
    scope("\"mutable\"", "storage.modifier"),

    scope("\";\"", "punctuation.terminator.statement"),
    scope("\"[\"", "punctuation.definition.begin.bracket.square"),
    scope("\"]\"", "punctuation.definition.end.bracket.square"),
    scope("access_specifier > \":\"", "punctuation.definition.visibility.colon"),
    scope("base_class_clause > \":\"", "punctuation.definition.inheritance.colon"),
    scope("base_class_clause > \",\"", "punctuation.definition.separator.class.comma"),
    scope("field_declaration > \",\"", "punctuation.separator.delimiter"),
    scope("parameter_list > \",\"", "punctuation.separator.delimiter"),
    scope("field_initializer_list > \":\"", "punctuation.definition.initialization.colon"),
    scope("field_initializer_list > \",\"", "punctuation.separator.delimiter"),
    scope("\"::\"", "punctuation.separator.method.double-colon"),
    scope("template_parameter_list > \"<\"", "punctuation.definition.template.bracket.angle"),
    scope("template_parameter_list > \">\"", "punctuation.definition.template.bracket.angle"),
    scope("template_argument_list > \">\"", "punctuation.definition.template.bracket.angle"),
    scope("template_argument_list > \"<\"", "punctuation.definition.template.bracket.angle"),
    scope("char_literal > \"\'\"", "punctuation.definition.string"),
    scope("string_literal > \"\\\"\"", "punctuation.definition.string"),
    scope("\"{\"", "punctuation.section.block.begin.bracket.curly"),
    scope("\"}\"", "punctuation.section.block.end.bracket.curly"),
    scope("\"(\"", "punctuation.section.parens.begin.bracket.round"),
    scope("\")\"", "punctuation.section.parens.end.bracket.round"),

    scope("\"sizeof\"", "keyword.operator.sizeof"),
    scope("\"new\"", "keyword.operator"),
    scope("\"delete\"", "keyword.operator"),
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
    scope("relational_expression > \"<\"", "keyword.operator"),
    scope("relational_expression > \">\"", "keyword.operator"),
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
    scope("conditional_expression > \":\"", "keyword.operator")
  );

  return grammar;
}
