#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_rust();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_rust() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "Rust",
    "source.rust",
    tree_sitter_rust()
  );

  grammar->setFileTypes(
    "rs"
  );

  grammar->setIncreaseIndentPattern(uR"""((?x)
     ^ .* \{ [^}"']* $
    |^ .* \( [^\)"']* $
    |^ \s* \{ \} $
  )""");
  grammar->setDecreaseIndentPattern(uR"""((?x)
     ^ \s* (\s* /[*] .* [*]/ \s*)* \}
    |^ \s* (\s* /[*] .* [*]/ \s*)* \)
  )""");

  grammar->setScopes(
    scope("type_identifier", "support.type"),
    scope("primitive_type", "support.type"),
    scope("field_identifier", "variable.other.member"),

    scope("line_comment", "comment.block"),
    scope("block_comment", "comment.block"),

    scope("identifier",
      match(u"^[A-Z\\d_]+$", "constant.other")
    ),

    scope(array(
      "identifier",
      "call_expression > identifier",
      "call_expression > field_expression > field_identifier",
      "call_expression > scoped_identifier > identifier:nth-child(2)"
    ),
      match(u"^[A-Z]", "entity.name.class")
    ),

    scope(array(
      "macro_invocation > identifier",
      "macro_invocation > \"!\"",
      "macro_definition > identifier",
      "call_expression > identifier",
      "call_expression > field_expression > field_identifier",
      "call_expression > scoped_identifier > identifier:nth-child(2)",
      "generic_function > identifier",
      "generic_function > field_expression > field_identifier",
      "generic_function > scoped_identifier > identifier",
      "function_item > identifier",
      "function_signature_item > identifier"
    ), "entity.name.function"),

    scope(array(
      "use_list > self",
      "scoped_use_list > self",
      "scoped_identifier > self",
      "crate",
      "super"
    ), "keyword.control"),

    scope("self", "variable.self"),

    scope(array(
      "use_wildcard > identifier:nth-child(0)",
      "use_wildcard > scoped_identifier > identifier:nth-child(2)",
      "scoped_type_identifier > identifier:nth-child(0)",
      "scoped_type_identifier > scoped_identifier:nth-child(0) > identifier",
      "scoped_identifier > identifier:nth-child(0)",
      "scoped_identifier > scoped_identifier:nth-child(0) > identifier",
      "use_declaration > identifier",
      "use_declaration > scoped_identifier > identifier",
      "use_list > identifier",
      "use_list > scoped_identifier > identifier",
      "meta_item > identifier"
    ),
      match(u"^[A-Z]", "support.type")
    ),

    scope("lifetime > identifier", "constant.variable"),

    scope("\"let\"", "storage.modifier"),
    scope("\"const\"", "storage.modifier"),
    scope("\"static\"", "storage.modifier"),
    scope("\"extern\"", "storage.modifier"),
    scope("\"fn\"", "storage.modifier"),
    scope("\"type\"", "storage.modifier"),
    scope("\"impl\"", "storage.modifier"),
    scope("\"dyn\"", "storage.modifier"),
    scope("\"trait\"", "storage.modifier"),
    scope("\"mod\"", "storage.modifier"),
    scope("\"pub\"", "storage.modifier"),
    scope("\"crate\"", "storage.modifier"),
    scope("\"default\"", "storage.modifier"),
    scope("\"struct\"", "storage.modifier"),
    scope("\"enum\"", "storage.modifier"),
    scope("\"union\"", "storage.modifier"),
    scope("mutable_specifier", "storage.modifier"),

    scope("\"unsafe\"", "keyword.control"),
    scope("\"use\"", "keyword.control"),
    scope("\"match\"", "keyword.control"),
    scope("\"if\"", "keyword.control"),
    scope("\"in\"", "keyword.control"),
    scope("\"else\"", "keyword.control"),
    scope("\"move\"", "keyword.control"),
    scope("\"while\"", "keyword.control"),
    scope("\"loop\"", "keyword.control"),
    scope("\"for\"", "keyword.control"),
    scope("\"let\"", "keyword.control"),
    scope("\"return\"", "keyword.control"),
    scope("\"continue\"", "keyword.control"),
    scope("\"break\"", "keyword.control"),
    scope("\"where\"", "keyword.control"),
    scope("\"ref\"", "keyword.control"),
    scope("\"macro_rules!\"", "keyword.control"),
    scope("\"async\"", "keyword.control"),
    scope("\"await\"", "keyword.control"),

    scope("\"as\"", "keyword.operator"),

    scope("char_literal", "string.quoted.single"),
    scope("string_literal", "string.quoted.double"),
    scope("raw_string_literal", "string.quoted.other"),
    scope("boolean_literal", "constant.language.boolean"),
    scope("integer_literal", "constant.numeric.decimal"),
    scope("float_literal", "constant.numeric.decimal"),
    scope("escape_sequence", "constant.character.escape"),

    scope("attribute_item, inner_attribute_item", "entity.other.attribute-name"),

    scope(array(
      "\"as\"",
      "\"*\"",
      "\"&\""
    ), "keyword.operator")
  );

  return grammar;
}
