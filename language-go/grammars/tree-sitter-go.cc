#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_go();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_go() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "Go",
    "source.go",
    tree_sitter_go()
  );

  grammar->setFileTypes(
    "go"
  );

  grammar->setIncreaseIndentPattern(u"^.*(\\bcase\\b.*:|\\bdefault\\b:|(\\b(func|if|else|switch|select|for|struct)\\b.*)?{[^}]*|\\([^)]*)$");
  grammar->setDecreaseIndentPattern(u"^\\s*(\\bcase\\b.*:|\\bdefault\\b:|}[),]?|\\)[,]?)$");
  grammar->setDecreaseNextIndentPattern(u"^\\s*[^\\s()}]+(?<m>[^()]*\\((?:\\g<m>[^()]*|[^()]*)\\))*[^()]*\\)[,]?$");

  grammar->setScopes(
    scope("source_file", "source.go"),

    scope("comment", "comment.block"),

    scope("\"var\"", "keyword.import"),
    scope("\"type\"", "keyword.type"),
    scope("\"func\"", "keyword.function"),
    scope("\"const\"", "keyword.const"),
    scope("\"struct\"", "keyword.struct"),
    scope("\"interface\"", "keyword.interface"),
    scope("\"import\"", "keyword.import"),
    scope("\"package\"", "keyword.package"),
    scope("\"map\"", "keyword.map"),
    scope("\"chan\"", "keyword.chan"),

    scope("type_identifier", "support.storage.type"),
    scope("field_identifier", "variable.other.object.property"),
    scope("package_identifier", "entity.name.package"),

    scope("\"if\"", "keyword.control"),
    scope("\"for\"", "keyword.control"),
    scope("\"else\"", "keyword.control"),
    scope("\"case\"", "keyword.control"),
    scope("\"break\"", "keyword.control"),
    scope("\"switch\"", "keyword.control"),
    scope("\"select\"", "keyword.control"),
    scope("\"return\"", "keyword.control"),
    scope("\"default\"", "keyword.control"),
    scope("\"continue\"", "keyword.control"),
    scope("\"goto\"", "keyword.control"),
    scope("\"fallthrough\"", "keyword.control"),
    scope("\"defer\"", "keyword.control"),
    scope("\"range\"", "keyword.control"),
    scope("\"go\"", "keyword.control"),

    scope("interpreted_string_literal", "string.quoted.double"),
    scope("raw_string_literal", "string.quoted.double"),
    scope("escape_sequence", "constant.character.escape"),
    scope("rune_literal", "constant.other.rune"),
    scope("int_literal", "constant.numeric.integer"),
    scope("float_literal", "constant.numeric.float"),
    scope("imaginary_literal", "constant.numeric.integer"),
    scope("nil", "constant.language.nil"),
    scope("false", "constant.language.false"),
    scope("true", "constant.language.true"),

    scope("call_expression > identifier", "entity.name.function"),
    scope("function_declaration > identifier", "entity.name.function"),
    scope("method_declaration > field_identifier", "entity.name.function"),
    scope("call_expression > selector_expression > field_identifier", "entity.name.function"),

    scope("\"+\"", "keyword.operator"),
    scope("\"-\"", "keyword.operator"),
    scope("\"*\"", "keyword.operator"),
    scope("\"/\"", "keyword.operator"),
    scope("\"%\"", "keyword.operator"),
    scope("\"++\"", "keyword.operator"),
    scope("\"--\"", "keyword.operator"),
    scope("\"==\"", "keyword.operator"),
    scope("\"!=\"", "keyword.operator"),
    scope("\">\"", "keyword.operator"),
    scope("\"<\"", "keyword.operator"),
    scope("\">=\"", "keyword.operator"),
    scope("\"<=\"", "keyword.operator"),
    scope("\"!\"", "keyword.operator"),
    scope("\"|\"", "keyword.operator"),
    scope("\"^\"", "keyword.operator"),
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
    scope("\":=\"", "keyword.operator"),
    scope("\"&\"", "keyword.operator"),
    scope("\"*\"", "keyword.operator"),
    scope("\"&&\"", "keyword.operator"),
    scope("\"||\"", "keyword.operator"),
    scope("\"...\"", "keyword.operator"),
    scope("\"<-\"", "keyword.operator")
  );

  return grammar;
}
