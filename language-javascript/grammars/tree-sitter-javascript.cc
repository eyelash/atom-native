#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_javascript();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_javascript() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "JavaScript",
    "source.js",
    tree_sitter_javascript()
  );

  grammar->setFileTypes(
    "js",
    "jsx"
  );

  grammar->setIncreaseIndentPattern(uR"""((?x)
      \{ [^}"']*(//.*)? $
    | \[ [^\]"']*(//.*)? $
    | \( [^)"']*(//.*)? $
  )""");
  grammar->setDecreaseIndentPattern(uR"""((?x)
      ^ \s* (\s* /[*] .* [*]/ \s*)* [}\])]
  )""");

  grammar->setScopes(
    scope("program", "source.js"),

    scope("property_identifier", array(
      match(
        u"^[$A-Z_]+$",
        "constant.other.property.js"
      ),

      "variable.other.object.property"
    )),

    scope("member_expression > property_identifier", "variable.other.object.property.unquoted"),

    scope("formal_parameters > identifier", "variable.parameter.function"),
    scope("formal_parameters > rest_parameter > identifier", "variable.parameter.rest.function"),

    scope("shorthand_property_identifier",
      match(
        u"^[$A-Z_]{2,}$",
        "constant.other"
      )
    ),

    scope(array(
      "class > identifier",
      "new_expression > identifier"
    ), "meta.class"),

    scope(array(
      "jsx_opening_element > identifier",
      "jsx_closing_element > identifier",
      "jsx_self_closing_element > identifier"
    ),
      match(
        u"^[A-Z]",
        "meta.class.component.jsx"
      )
    ),

    scope("call_expression > identifier", match(u"^[A-Z]", "meta.class")),
    scope("arrow_function > identifier:nth-child(0)", "variable.parameter.function"),

    scope("function > identifier", "entity.name.function"),
    scope("function_declaration > identifier", "entity.name.function"),
    scope("generator_function > identifier", "entity.name.function"),

    scope("call_expression > identifier", array(
      match(u"^require$", "support.function"),
      "entity.name.function"
    )),

    scope("call_expression > super", "support.function.super"),

    scope("method_definition > property_identifier", "entity.name.function"),
    scope("call_expression > member_expression > property_identifier", "entity.name.function"),

    scope("identifier", array(
      match(
        u"^(global|globalThis|module|exports|__filename|__dirname)$",
        "support.variable"
      ),
      match(
        u"^(window|self|frames|event|document|performance|screen|navigator|console)$",
        "support.variable.dom"
      ),
      exact(
        u"require",
        "support.function"
      ),
      match(
        u"^[$A-Z_]{2,}$",
        "constant.other"
      ),
      match(
        u"^[A-Z]",
        "meta.class"
      )
    )),

    scope("number", "constant.numeric"),
    scope("string", "string.quoted"),
    scope("regex", "string.regexp"),
    scope("escape_sequence", "constant.character.escape"),
    scope("template_string", "string.quoted.template"),
    scope("undefined", "constant.language"),
    scope("null", "constant.language.null"),
    scope("true", "constant.language.boolean.true"),
    scope("false", "constant.language.boolean.false"),
    scope("comment", array(
      match(
        u"^//",
        "comment.line"
      ),
      "comment.block"
    )),
    scope("hash_bang_line", "comment.block"),

    scope(array(
      "jsx_expression > \"{\"",
      "jsx_expression > \"}\"",
      "template_substitution > \"${\"",
      "template_substitution > \"}\""
    ), "punctuation.section.embedded"),
    scope("template_substitution", "embedded.source"),

    scope("\"(\"", "punctuation.definition.parameters.begin.bracket.round"),
    scope("\")\"", "punctuation.definition.parameters.end.bracket.round"),
    scope("\"{\"", "punctuation.definition.function.body.begin.bracket.curly"),
    scope("\"}\"", "punctuation.definition.function.body.end.bracket.curly"),
    scope("\";\"", "punctuation.terminator.statement.semicolon"),
    scope("\"[\"", "punctuation.definition.array.begin.bracket.square"),
    scope("\"]\"", "punctuation.definition.array.end.bracket.square"),

    scope("\"var\"", "storage.type"),
    scope("\"let\"", "storage.type"),
    scope("\"class\"", "storage.type"),
    scope("\"extends\"", "storage.modifier"),
    scope("\"const\"", "storage.modifier"),
    scope("\"static\"", "storage.modifier"),
    scope("\"function\"", "storage.type.function"),
    scope("\"=>\"", "storage.type.function.arrow"),

    scope("\"=\"", "keyword.operator.js"),
    scope("\"+=\"", "keyword.operator.js"),
    scope("\"-=\"", "keyword.operator.js"),
    scope("\"*=\"", "keyword.operator.js"),
    scope("\"/=\"", "keyword.operator.js"),
    scope("\"%=\"", "keyword.operator.js"),
    scope("\"<<=\"", "keyword.operator.js"),
    scope("\">>=\"", "keyword.operator.js"),
    scope("\">>>=\"", "keyword.operator.js"),
    scope("\"&=\"", "keyword.operator.js"),
    scope("\"^=\"", "keyword.operator.js"),
    scope("\"|=\"", "keyword.operator.js"),
    scope("\"!\"", "keyword.operator.js"),
    scope("\"+\"", "keyword.operator.js"),
    scope("\"-\"", "keyword.operator.js"),
    scope("\"*\"", "keyword.operator.js"),
    scope("\"/\"", "keyword.operator.js"),
    scope("\"%\"", "keyword.operator.js"),
    scope("\"==\"", "keyword.operator.js"),
    scope("\"===\"", "keyword.operator.js"),
    scope("\"!=\"", "keyword.operator.js"),
    scope("\"!==\"", "keyword.operator.js"),
    scope("\">=\"", "keyword.operator.js"),
    scope("\"<=\"", "keyword.operator.js"),
    scope("\">\"", "keyword.operator.js"),
    scope("\"<\"", "keyword.operator.js"),
    scope("\":\"", "keyword.operator.js"),
    scope("\"?\"", "keyword.operator.js"),
    scope("\"&&\"", "keyword.operator.js"),
    scope("\"||\"", "keyword.operator.js"),
    scope("\"&\"", "keyword.operator.js"),
    scope("\"~\"", "keyword.operator.js"),
    scope("\"^\"", "keyword.operator.js"),
    scope("\">>\"", "keyword.operator.js"),
    scope("\">>>\"", "keyword.operator.js"),
    scope("\"<<\"", "keyword.operator.js"),
    scope("\"|\"", "keyword.operator.js"),
    scope("\"++\"", "keyword.operator.js"),
    scope("\"--\"", "keyword.operator.js"),
    scope("\"...\"", "keyword.operator.spread.js"),

    scope("\"in\"", "keyword.operator.in"),
    scope("\"instanceof\"", "keyword.operator.instanceof"),
    scope("\"of\"", "keyword.operator.of"),
    scope("\"new\"", "keyword.operator.new"),
    scope("\"typeof\"", "keyword.operator.typeof"),

    scope("\"get\"", "keyword.operator.setter"),
    scope("\"set\"", "keyword.operator.setter"),

    scope("\".\"", "meta.delimiter.period"),
    scope("\",\"", "meta.delimiter.comma"),

    scope("\"as\"", "keyword.control"),
    scope("\"if\"", "keyword.control"),
    scope("\"do\"", "keyword.control"),
    scope("\"else\"", "keyword.control"),
    scope("\"while\"", "keyword.control"),
    scope("\"for\"", "keyword.control"),
    scope("\"return\"", "keyword.control"),
    scope("\"break\"", "keyword.control"),
    scope("\"continue\"", "keyword.control"),
    scope("\"throw\"", "keyword.control"),
    scope("\"try\"", "keyword.control"),
    scope("\"catch\"", "keyword.control"),
    scope("\"finally\"", "keyword.control"),
    scope("\"switch\"", "keyword.control"),
    scope("\"case\"", "keyword.control"),
    scope("\"default\"", "keyword.control"),
    scope("\"export\"", "keyword.control"),
    scope("\"import\"", "keyword.control"),
    scope("\"from\"", "keyword.control"),
    scope("\"yield\"", "keyword.control"),
    scope("\"async\"", "keyword.control"),
    scope("\"await\"", "keyword.control"),
    scope("\"debugger\"", "keyword.control"),
    scope("\"delete\"", "keyword.control"),

    scope("jsx_attribute > property_identifier", "entity.other.attribute-name"),
    scope("jsx_opening_element > identifier", "entity.name.tag"),
    scope("jsx_closing_element > identifier", "entity.name.tag"),
    scope("jsx_self_closing_element > identifier", "entity.name.tag")
  );

  return grammar;
}
