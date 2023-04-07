#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_rust();

extern "C" TreeSitterGrammar *atom_language_rust() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "Rust",
    "source.rust",
    tree_sitter_rust()
  );

  grammar->addFileTypes(
    "rs"
  );

  grammar->setIncreaseIndentPattern(uR"--((?x)
     ^ .* \{ [^}"']* $
    |^ .* \( [^\)"']* $
    |^ \s* \{ \} $
  )--");
  grammar->setDecreaseIndentPattern(uR"--((?x)
     ^ \s* (\s* /[*] .* [*]/ \s*)* \}
    |^ \s* (\s* /[*] .* [*]/ \s*)* \)
  )--");

  grammar->addScopes("type_identifier", "support.type");
  grammar->addScopes("primitive_type", "support.type");
  grammar->addScopes("field_identifier", "variable.other.member");

  grammar->addScopes("line_comment", "comment.block");
  grammar->addScopes("block_comment", "comment.block");

  grammar->addScopes("identifier",
    TreeSitterGrammar::Match{u"^[A-Z\\d_]+$", "constant.other"}
  );

  grammar->addScopes({
    "identifier",
    "call_expression > identifier",
    "call_expression > field_expression > field_identifier",
    "call_expression > scoped_identifier > identifier:nth-child(2)"
  },
    TreeSitterGrammar::Match{u"^[A-Z]", "entity.name.class"}
  );

  grammar->addScopes({
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
    "function_signature_item > identifier",
  }, "entity.name.function");

  grammar->addScopes({
    "use_list > self",
    "scoped_use_list > self",
    "scoped_identifier > self",
    "crate",
    "super",
  }, "keyword.control");

  grammar->addScopes("self", "variable.self");

  grammar->addScopes({
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
    "meta_item > identifier",
  },
    TreeSitterGrammar::Match{u"^[A-Z]", "support.type"}
  );

  grammar->addScopes("lifetime > identifier", "constant.variable");

  grammar->addScopes("\"let\"", "storage.modifier");
  grammar->addScopes("\"const\"", "storage.modifier");
  grammar->addScopes("\"static\"", "storage.modifier");
  grammar->addScopes("\"extern\"", "storage.modifier");
  grammar->addScopes("\"fn\"", "storage.modifier");
  grammar->addScopes("\"type\"", "storage.modifier");
  grammar->addScopes("\"impl\"", "storage.modifier");
  grammar->addScopes("\"dyn\"", "storage.modifier");
  grammar->addScopes("\"trait\"", "storage.modifier");
  grammar->addScopes("\"mod\"", "storage.modifier");
  grammar->addScopes("\"pub\"", "storage.modifier");
  grammar->addScopes("\"crate\"", "storage.modifier");
  grammar->addScopes("\"default\"", "storage.modifier");
  grammar->addScopes("\"struct\"", "storage.modifier");
  grammar->addScopes("\"enum\"", "storage.modifier");
  grammar->addScopes("\"union\"", "storage.modifier");
  grammar->addScopes("mutable_specifier", "storage.modifier");

  grammar->addScopes("\"unsafe\"", "keyword.control");
  grammar->addScopes("\"use\"", "keyword.control");
  grammar->addScopes("\"match\"", "keyword.control");
  grammar->addScopes("\"if\"", "keyword.control");
  grammar->addScopes("\"in\"", "keyword.control");
  grammar->addScopes("\"else\"", "keyword.control");
  grammar->addScopes("\"move\"", "keyword.control");
  grammar->addScopes("\"while\"", "keyword.control");
  grammar->addScopes("\"loop\"", "keyword.control");
  grammar->addScopes("\"for\"", "keyword.control");
  grammar->addScopes("\"let\"", "keyword.control");
  grammar->addScopes("\"return\"", "keyword.control");
  grammar->addScopes("\"continue\"", "keyword.control");
  grammar->addScopes("\"break\"", "keyword.control");
  grammar->addScopes("\"where\"", "keyword.control");
  grammar->addScopes("\"ref\"", "keyword.control");
  grammar->addScopes("\"macro_rules!\"", "keyword.control");
  grammar->addScopes("\"async\"", "keyword.control");
  grammar->addScopes("\"await\"", "keyword.control");

  grammar->addScopes("\"as\"", "keyword.operator");

  grammar->addScopes("char_literal", "string.quoted.single");
  grammar->addScopes("string_literal", "string.quoted.double");
  grammar->addScopes("raw_string_literal", "string.quoted.other");
  grammar->addScopes("boolean_literal", "constant.language.boolean");
  grammar->addScopes("integer_literal", "constant.numeric.decimal");
  grammar->addScopes("float_literal", "constant.numeric.decimal");
  grammar->addScopes("escape_sequence", "constant.character.escape");

  grammar->addScopes("attribute_item, inner_attribute_item", "entity.other.attribute-name");

  grammar->addScopes({
    "\"as\"",
    "\"*\"",
    "\"&\"",
  }, "keyword.operator");

  return grammar;
}
