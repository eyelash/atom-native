#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_go();

extern "C" TreeSitterGrammar *atom_language_go() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "Go",
    "source.go",
    tree_sitter_go()
  );

  grammar->addFileTypes(
    "go"
  );

  grammar->setIncreaseIndentPattern(u"^.*(\\bcase\\b.*:|\\bdefault\\b:|(\\b(func|if|else|switch|select|for|struct)\\b.*)?{[^}]*|\\([^)]*)$");
  grammar->setDecreaseIndentPattern(u"^\\s*(\\bcase\\b.*:|\\bdefault\\b:|}[),]?|\\)[,]?)$");
  grammar->setDecreaseNextIndentPattern(u"^\\s*[^\\s()}]+(?<m>[^()]*\\((?:\\g<m>[^()]*|[^()]*)\\))*[^()]*\\)[,]?$");

  grammar->addScopes("source_file", "source.go");

  grammar->addScopes("comment", "comment.block");

  grammar->addScopes("\"var\"", "keyword.import");
  grammar->addScopes("\"type\"", "keyword.type");
  grammar->addScopes("\"func\"", "keyword.function");
  grammar->addScopes("\"const\"", "keyword.const");
  grammar->addScopes("\"struct\"", "keyword.struct");
  grammar->addScopes("\"interface\"", "keyword.interface");
  grammar->addScopes("\"import\"", "keyword.import");
  grammar->addScopes("\"package\"", "keyword.package");
  grammar->addScopes("\"map\"", "keyword.map");
  grammar->addScopes("\"chan\"", "keyword.chan");

  grammar->addScopes("type_identifier", "support.storage.type");
  grammar->addScopes("field_identifier", "variable.other.object.property");
  grammar->addScopes("package_identifier", "entity.name.package");

  grammar->addScopes("\"if\"", "keyword.control");
  grammar->addScopes("\"for\"", "keyword.control");
  grammar->addScopes("\"else\"", "keyword.control");
  grammar->addScopes("\"case\"", "keyword.control");
  grammar->addScopes("\"break\"", "keyword.control");
  grammar->addScopes("\"switch\"", "keyword.control");
  grammar->addScopes("\"select\"", "keyword.control");
  grammar->addScopes("\"return\"", "keyword.control");
  grammar->addScopes("\"default\"", "keyword.control");
  grammar->addScopes("\"continue\"", "keyword.control");
  grammar->addScopes("\"goto\"", "keyword.control");
  grammar->addScopes("\"fallthrough\"", "keyword.control");
  grammar->addScopes("\"defer\"", "keyword.control");
  grammar->addScopes("\"range\"", "keyword.control");
  grammar->addScopes("\"go\"", "keyword.control");

  grammar->addScopes("interpreted_string_literal", "string.quoted.double");
  grammar->addScopes("raw_string_literal", "string.quoted.double");
  grammar->addScopes("escape_sequence", "constant.character.escape");
  grammar->addScopes("rune_literal", "constant.other.rune");
  grammar->addScopes("int_literal", "constant.numeric.integer");
  grammar->addScopes("float_literal", "constant.numeric.float");
  grammar->addScopes("imaginary_literal", "constant.numeric.integer");
  grammar->addScopes("nil", "constant.language.nil");
  grammar->addScopes("false", "constant.language.false");
  grammar->addScopes("true", "constant.language.true");

  grammar->addScopes("call_expression > identifier", "entity.name.function");
  grammar->addScopes("function_declaration > identifier", "entity.name.function");
  grammar->addScopes("method_declaration > field_identifier", "entity.name.function");
  grammar->addScopes("call_expression > selector_expression > field_identifier", "entity.name.function");

  grammar->addScopes("\"+\"", "keyword.operator");
  grammar->addScopes("\"-\"", "keyword.operator");
  grammar->addScopes("\"*\"", "keyword.operator");
  grammar->addScopes("\"/\"", "keyword.operator");
  grammar->addScopes("\"%\"", "keyword.operator");
  grammar->addScopes("\"++\"", "keyword.operator");
  grammar->addScopes("\"--\"", "keyword.operator");
  grammar->addScopes("\"==\"", "keyword.operator");
  grammar->addScopes("\"!=\"", "keyword.operator");
  grammar->addScopes("\">\"", "keyword.operator");
  grammar->addScopes("\"<\"", "keyword.operator");
  grammar->addScopes("\">=\"", "keyword.operator");
  grammar->addScopes("\"<=\"", "keyword.operator");
  grammar->addScopes("\"!\"", "keyword.operator");
  grammar->addScopes("\"|\"", "keyword.operator");
  grammar->addScopes("\"^\"", "keyword.operator");
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
  grammar->addScopes("\":=\"", "keyword.operator");
  grammar->addScopes("\"&\"", "keyword.operator");
  grammar->addScopes("\"*\"", "keyword.operator");
  grammar->addScopes("\"&&\"", "keyword.operator");
  grammar->addScopes("\"||\"", "keyword.operator");
  grammar->addScopes("\"...\"", "keyword.operator");
  grammar->addScopes("\"<-\"", "keyword.operator");

  return grammar;
}
