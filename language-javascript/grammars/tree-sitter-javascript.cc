#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_javascript();

extern "C" TreeSitterGrammar *atom_language_javascript() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "JavaScript",
    "source.js",
    tree_sitter_javascript()
  );

  grammar->addFileTypes(
    "js",
    "jsx"
  );

  grammar->setIncreaseIndentPattern(uR"--((?x)
      \{ [^}"']*(//.*)? $
    | \[ [^\]"']*(//.*)? $
    | \( [^)"']*(//.*)? $
  )--");
  grammar->setDecreaseIndentPattern(uR"--((?x)
      ^ \s* (\s* /[*] .* [*]/ \s*)* [}\])]
  )--");

  grammar->addScopes("program", "source.js");

  grammar->addScopes("property_identifier",
    TreeSitterGrammar::Match{
      u"^[$A-Z_]+$",
      "constant.other.property.js"
    },

    "variable.other.object.property"
  );

  grammar->addScopes("member_expression > property_identifier", "variable.other.object.property.unquoted");

  grammar->addScopes("formal_parameters > identifier", "variable.parameter.function");
  grammar->addScopes("formal_parameters > rest_parameter > identifier", "variable.parameter.rest.function");

  grammar->addScopes("shorthand_property_identifier",
    TreeSitterGrammar::Match{
      u"^[$A-Z_]{2,}$",
      "constant.other"
    }
  );

  grammar->addScopes({
    "class > identifier",
    "new_expression > identifier"
  }, "meta.class");

  grammar->addScopes({
    "jsx_opening_element > identifier",
    "jsx_closing_element > identifier",
    "jsx_self_closing_element > identifier"
  },
    TreeSitterGrammar::Match{
      u"^[A-Z]",
      "meta.class.component.jsx"
    }
  );

  grammar->addScopes("call_expression > identifier", TreeSitterGrammar::Match{u"^[A-Z]", "meta.class"});
  grammar->addScopes("arrow_function > identifier:nth-child(0)", "variable.parameter.function");

  grammar->addScopes("function > identifier", "entity.name.function");
  grammar->addScopes("function_declaration > identifier", "entity.name.function");
  grammar->addScopes("generator_function > identifier", "entity.name.function");

  grammar->addScopes("call_expression > identifier",
    TreeSitterGrammar::Match{u"^require$", "support.function"},
    "entity.name.function"
  );

  grammar->addScopes("call_expression > super", "support.function.super");

  grammar->addScopes("method_definition > property_identifier", "entity.name.function");
  grammar->addScopes("call_expression > member_expression > property_identifier", "entity.name.function");

  grammar->addScopes("identifier",
    TreeSitterGrammar::Match{
      u"^(global|globalThis|module|exports|__filename|__dirname)$",
      "support.variable"
    },
    TreeSitterGrammar::Match{
      u"^(window|self|frames|event|document|performance|screen|navigator|console)$",
      "support.variable.dom"
    },
    TreeSitterGrammar::Exact{
      u"require",
      "support.function"
    },
    TreeSitterGrammar::Match{
      u"^[$A-Z_]{2,}$",
      "constant.other"
    },
    TreeSitterGrammar::Match{
      u"^[A-Z]",
      "meta.class"
    }
  );

  grammar->addScopes("number", "constant.numeric");
  grammar->addScopes("string", "string.quoted");
  grammar->addScopes("regex", "string.regexp");
  grammar->addScopes("escape_sequence", "constant.character.escape");
  grammar->addScopes("template_string", "string.quoted.template");
  grammar->addScopes("undefined", "constant.language");
  grammar->addScopes("null", "constant.language.null");
  grammar->addScopes("true", "constant.language.boolean.true");
  grammar->addScopes("false", "constant.language.boolean.false");
  grammar->addScopes("comment",
    TreeSitterGrammar::Match{
      u"^//",
      "comment.line"
    },
    "comment.block"
  );
  grammar->addScopes("hash_bang_line", "comment.block");

  grammar->addScopes({
    "jsx_expression > \"{\"",
    "jsx_expression > \"}\"",
    "template_substitution > \"${\"",
    "template_substitution > \"}\""
  }, "punctuation.section.embedded");
  grammar->addScopes("template_substitution", "embedded.source");

  grammar->addScopes("\"(\"", "punctuation.definition.parameters.begin.bracket.round");
  grammar->addScopes("\")\"", "punctuation.definition.parameters.end.bracket.round");
  grammar->addScopes("\"{\"", "punctuation.definition.function.body.begin.bracket.curly");
  grammar->addScopes("\"}\"", "punctuation.definition.function.body.end.bracket.curly");
  grammar->addScopes("\";\"", "punctuation.terminator.statement.semicolon");
  grammar->addScopes("\"[\"", "punctuation.definition.array.begin.bracket.square");
  grammar->addScopes("\"]\"", "punctuation.definition.array.end.bracket.square");

  grammar->addScopes("\"var\"", "storage.type");
  grammar->addScopes("\"let\"", "storage.type");
  grammar->addScopes("\"class\"", "storage.type");
  grammar->addScopes("\"extends\"", "storage.modifier");
  grammar->addScopes("\"const\"", "storage.modifier");
  grammar->addScopes("\"static\"", "storage.modifier");
  grammar->addScopes("\"function\"", "storage.type.function");
  grammar->addScopes("\"=>\"", "storage.type.function.arrow");

  grammar->addScopes("\"=\"", "keyword.operator.js");
  grammar->addScopes("\"+=\"", "keyword.operator.js");
  grammar->addScopes("\"-=\"", "keyword.operator.js");
  grammar->addScopes("\"*=\"", "keyword.operator.js");
  grammar->addScopes("\"/=\"", "keyword.operator.js");
  grammar->addScopes("\"%=\"", "keyword.operator.js");
  grammar->addScopes("\"<<=\"", "keyword.operator.js");
  grammar->addScopes("\">>=\"", "keyword.operator.js");
  grammar->addScopes("\">>>=\"", "keyword.operator.js");
  grammar->addScopes("\"&=\"", "keyword.operator.js");
  grammar->addScopes("\"^=\"", "keyword.operator.js");
  grammar->addScopes("\"|=\"", "keyword.operator.js");
  grammar->addScopes("\"!\"", "keyword.operator.js");
  grammar->addScopes("\"+\"", "keyword.operator.js");
  grammar->addScopes("\"-\"", "keyword.operator.js");
  grammar->addScopes("\"*\"", "keyword.operator.js");
  grammar->addScopes("\"/\"", "keyword.operator.js");
  grammar->addScopes("\"%\"", "keyword.operator.js");
  grammar->addScopes("\"==\"", "keyword.operator.js");
  grammar->addScopes("\"===\"", "keyword.operator.js");
  grammar->addScopes("\"!=\"", "keyword.operator.js");
  grammar->addScopes("\"!==\"", "keyword.operator.js");
  grammar->addScopes("\">=\"", "keyword.operator.js");
  grammar->addScopes("\"<=\"", "keyword.operator.js");
  grammar->addScopes("\">\"", "keyword.operator.js");
  grammar->addScopes("\"<\"", "keyword.operator.js");
  grammar->addScopes("\":\"", "keyword.operator.js");
  grammar->addScopes("\"?\"", "keyword.operator.js");
  grammar->addScopes("\"&&\"", "keyword.operator.js");
  grammar->addScopes("\"||\"", "keyword.operator.js");
  grammar->addScopes("\"&\"", "keyword.operator.js");
  grammar->addScopes("\"~\"", "keyword.operator.js");
  grammar->addScopes("\"^\"", "keyword.operator.js");
  grammar->addScopes("\">>\"", "keyword.operator.js");
  grammar->addScopes("\">>>\"", "keyword.operator.js");
  grammar->addScopes("\"<<\"", "keyword.operator.js");
  grammar->addScopes("\"|\"", "keyword.operator.js");
  grammar->addScopes("\"++\"", "keyword.operator.js");
  grammar->addScopes("\"--\"", "keyword.operator.js");
  grammar->addScopes("\"...\"", "keyword.operator.spread.js");

  grammar->addScopes("\"in\"", "keyword.operator.in");
  grammar->addScopes("\"instanceof\"", "keyword.operator.instanceof");
  grammar->addScopes("\"of\"", "keyword.operator.of");
  grammar->addScopes("\"new\"", "keyword.operator.new");
  grammar->addScopes("\"typeof\"", "keyword.operator.typeof");

  grammar->addScopes("\"get\"", "keyword.operator.setter");
  grammar->addScopes("\"set\"", "keyword.operator.setter");

  grammar->addScopes("\".\"", "meta.delimiter.period");
  grammar->addScopes("\",\"", "meta.delimiter.comma");

  grammar->addScopes("\"as\"", "keyword.control");
  grammar->addScopes("\"if\"", "keyword.control");
  grammar->addScopes("\"do\"", "keyword.control");
  grammar->addScopes("\"else\"", "keyword.control");
  grammar->addScopes("\"while\"", "keyword.control");
  grammar->addScopes("\"for\"", "keyword.control");
  grammar->addScopes("\"return\"", "keyword.control");
  grammar->addScopes("\"break\"", "keyword.control");
  grammar->addScopes("\"continue\"", "keyword.control");
  grammar->addScopes("\"throw\"", "keyword.control");
  grammar->addScopes("\"try\"", "keyword.control");
  grammar->addScopes("\"catch\"", "keyword.control");
  grammar->addScopes("\"finally\"", "keyword.control");
  grammar->addScopes("\"switch\"", "keyword.control");
  grammar->addScopes("\"case\"", "keyword.control");
  grammar->addScopes("\"default\"", "keyword.control");
  grammar->addScopes("\"export\"", "keyword.control");
  grammar->addScopes("\"import\"", "keyword.control");
  grammar->addScopes("\"from\"", "keyword.control");
  grammar->addScopes("\"yield\"", "keyword.control");
  grammar->addScopes("\"async\"", "keyword.control");
  grammar->addScopes("\"await\"", "keyword.control");
  grammar->addScopes("\"debugger\"", "keyword.control");
  grammar->addScopes("\"delete\"", "keyword.control");

  grammar->addScopes("jsx_attribute > property_identifier", "entity.other.attribute-name");
  grammar->addScopes("jsx_opening_element > identifier", "entity.name.tag");
  grammar->addScopes("jsx_closing_element > identifier", "entity.name.tag");
  grammar->addScopes("jsx_self_closing_element > identifier", "entity.name.tag");

  return grammar;
}
