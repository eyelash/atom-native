#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_cpp();

extern "C" TreeSitterGrammar *atom_language_cpp() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "C++",
    "source.cpp",
    tree_sitter_cpp()
  );

  grammar->addFileType("cc");
  grammar->addFileType("cpp");
  grammar->addFileType("cp");
  grammar->addFileType("cxx");
  grammar->addFileType("c++");
  grammar->addFileType("cu");
  grammar->addFileType("cuh");
  grammar->addFileType("h");
  grammar->addFileType("hh");
  grammar->addFileType("hpp");
  grammar->addFileType("hxx");
  grammar->addFileType("h++");
  grammar->addFileType("inl");
  grammar->addFileType("ino");
  grammar->addFileType("ipp");
  grammar->addFileType("tcc");
  grammar->addFileType("tpp");

  grammar->addScope("translation_unit", "source.cpp");
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
  grammar->addScope("\"class\"", "keyword.control");
  grammar->addScope("\"using\"", "keyword.control");
  grammar->addScope("\"namespace\"", "keyword.control");
  grammar->addScope("\"template\"", "keyword.control");
  grammar->addScope("\"typename\"", "keyword.control");
  grammar->addScope("\"try\"", "keyword.control");
  grammar->addScope("\"catch\"", "keyword.control");
  grammar->addScope("\"throw\"", "keyword.control");
  grammar->addScope("\"__attribute__\"", "keyword.attribute");

  grammar->addScope("preproc_function_def > identifier:nth-child(1)", "entity.name.function.preprocessor");
  grammar->addScope("preproc_arg", "meta.preprocessor.macro");
  grammar->addScope("preproc_directive", "keyword.control.directive");

  /*'template_function > identifier': [
    {
      match: '^(static|const|dynamic|reinterpret)_cast$'
      scopes: 'keyword.operator'
    }
  ]*/

  grammar->addScope(R"(
    call_expression > identifier,
    call_expression > field_expression > field_identifier,
    call_expression > scoped_identifier > identifier,
    template_function > identifier,
    template_function > scoped_identifier > identifier,
    template_method > field_identifier,
    function_declarator > identifier,
    function_declarator > field_identifier,
    function_declarator > scoped_identifier > identifier,
    destructor_name > identifier
  )", "entity.name.function");

  grammar->addScope("statement_identifier", "constant.variable");

  grammar->addScope("field_identifier", "variable.other.member");

  grammar->addScope("type_identifier", "support.storage.type");
  grammar->addScope("primitive_type", "support.storage.type");
  grammar->addScope("\"unsigned\"", "support.storage.type");
  grammar->addScope("\"signed\"", "support.storage.type");
  grammar->addScope("\"short\"", "support.storage.type");
  grammar->addScope("\"long\"", "support.storage.type");
  grammar->addScope("auto", "support.storage.type");

  grammar->addScope("char_literal", "string.quoted.single");
  grammar->addScope("string_literal", "string.quoted.double");
  grammar->addScope("system_lib_string", "string.quoted.other");
  grammar->addScope("raw_string_literal", "string.quoted.other");
  grammar->addScope("escape_sequence", "constant.character.escape");
  grammar->addScope("preproc_include > string_literal > escape_sequence", "string.quoted.double");

  grammar->addScope("number_literal", "constant.numeric.decimal");
  grammar->addScope("null", "constant.language.null");
  grammar->addScope("nullptr", "constant.language.null");
  grammar->addScope("true", "constant.language.boolean");
  grammar->addScope("false", "constant.language.boolean");

  grammar->addScope("\"extern\"", "storage.modifier");
  grammar->addScope("\"static\"", "storage.modifier");
  grammar->addScope("\"register\"", "storage.modifier");
  grammar->addScope("\"friend\"", "storage.modifier");
  grammar->addScope("\"inline\"", "storage.modifier");
  grammar->addScope("\"explicit\"", "storage.modifier");
  grammar->addScope("\"const\"", "storage.modifier");
  grammar->addScope("\"constexpr\"", "storage.modifier");
  grammar->addScope("\"volatile\"", "storage.modifier");
  grammar->addScope("\"restrict\"", "storage.modifier");
  grammar->addScope("function_specifier", "storage.modifier");
  grammar->addScope("\"public\"", "storage.modifier");
  grammar->addScope("\"private\"", "storage.modifier");
  grammar->addScope("\"protected\"", "storage.modifier");
  grammar->addScope("\"final\"", "storage.modifier");
  grammar->addScope("\"override\"", "storage.modifier");
  grammar->addScope("\"virtual\"", "storage.modifier");
  grammar->addScope("\"noexcept\"", "storage.modifier");
  grammar->addScope("\"mutable\"", "storage.modifier");

  grammar->addScope("\";\"", "punctuation.terminator.statement");
  grammar->addScope("\"[\"", "punctuation.definition.begin.bracket.square");
  grammar->addScope("\"]\"", "punctuation.definition.end.bracket.square");
  grammar->addScope("access_specifier > \":\"", "punctuation.definition.visibility.colon");
  grammar->addScope("base_class_clause > \":\"", "punctuation.definition.inheritance.colon");
  grammar->addScope("base_class_clause > \",\"", "punctuation.definition.separator.class.comma");
  grammar->addScope("field_declaration > \",\"", "punctuation.separator.delimiter");
  grammar->addScope("parameter_list > \",\"", "punctuation.separator.delimiter");
  grammar->addScope("field_initializer_list > \":\"", "punctuation.definition.initialization.colon");
  grammar->addScope("field_initializer_list > \",\"", "punctuation.separator.delimiter");
  grammar->addScope("\"::\"", "punctuation.separator.method.double-colon");
  grammar->addScope("template_parameter_list > \"<\"", "punctuation.definition.template.bracket.angle");
  grammar->addScope("template_parameter_list > \">\"", "punctuation.definition.template.bracket.angle");
  grammar->addScope("template_argument_list > \">\"", "punctuation.definition.template.bracket.angle");
  grammar->addScope("template_argument_list > \"<\"", "punctuation.definition.template.bracket.angle");
  grammar->addScope("char_literal > \"\'\"", "punctuation.definition.string");
  grammar->addScope("string_literal > \"\\\"\"", "punctuation.definition.string");
  grammar->addScope("\"{\"", "punctuation.section.block.begin.bracket.curly");
  grammar->addScope("\"}\"", "punctuation.section.block.end.bracket.curly");
  grammar->addScope("\"(\"", "punctuation.section.parens.begin.bracket.round");
  grammar->addScope("\")\"", "punctuation.section.parens.end.bracket.round");

  grammar->addScope("\"sizeof\"", "keyword.operator.sizeof");
  grammar->addScope("\"new\"", "keyword.operator");
  grammar->addScope("\"delete\"", "keyword.operator");
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
  grammar->addScope("relational_expression > \"<\"", "keyword.operator");
  grammar->addScope("relational_expression > \">\"", "keyword.operator");
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
  grammar->addScope("conditional_expression > \":\"", "keyword.operator");

  return grammar;
}
