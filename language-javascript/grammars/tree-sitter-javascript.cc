#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_javascript();

extern "C" TreeSitterGrammar *atom_language_javascript() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "JavaScript",
    "source.js",
    tree_sitter_javascript()
  );

  grammar->addFileType("js");
  grammar->addFileType("jsx");

  grammar->addScope("program", "source.js");

  /*'property_identifier': [
    {
      match: '^[\$A-Z_]+$',
      scopes: 'constant.other.property.js'
    }

    'variable.other.object.property'
  ]*/

  grammar->addScope("member_expression > property_identifier", "variable.other.object.property.unquoted");

  grammar->addScope("formal_parameters > identifier", "variable.parameter.function");
  grammar->addScope("formal_parameters > rest_parameter > identifier", "variable.parameter.rest.function");

  /*'shorthand_property_identifier': [
    {
      match: '^[\$A-Z_]{2,}$',
      scopes: 'constant.other'
    }
  ]*/

  grammar->addScope(R"(
    class > identifier,
    new_expression > identifier
  )", "meta.class");

  /*'
    jsx_opening_element > identifier,
    jsx_closing_element > identifier,
    jsx_self_closing_element > identifier
  ': [
    {
      match: '^[A-Z]',
      scopes: 'meta.class.component.jsx'
    }
  ]*/

  //'call_expression > identifier': {match: '^[A-Z]', scopes: 'meta.class'}
  grammar->addScope("arrow_function > identifier:nth-child(0)", "variable.parameter.function");

  grammar->addScope("function > identifier", "entity.name.function");
  grammar->addScope("function_declaration > identifier", "entity.name.function");
  grammar->addScope("generator_function > identifier", "entity.name.function");

  /*'call_expression > identifier': [
    {match: '^require$', scopes: 'support.function'},
    'entity.name.function'
  ]*/

  grammar->addScope("call_expression > super", "support.function.super");

  grammar->addScope("method_definition > property_identifier", "entity.name.function");
  grammar->addScope("call_expression > member_expression > property_identifier", "entity.name.function");

  /*'identifier': [
    {
      match: '^(global|globalThis|module|exports|__filename|__dirname)$',
      scopes: 'support.variable'
    },
    {
      match: '^(window|self|frames|event|document|performance|screen|navigator|console)$'
      scopes: 'support.variable.dom'
    },
    {
      exact: 'require',
      scopes: 'support.function'
    },
    {
      match: '^[\$A-Z_]{2,}$',
      scopes: 'constant.other'
    },
    {
      match: '^[A-Z]',
      scopes: 'meta.class'
    },
  ]*/

  grammar->addScope("number", "constant.numeric");
  grammar->addScope("string", "string.quoted");
  grammar->addScope("regex", "string.regexp");
  grammar->addScope("escape_sequence", "constant.character.escape");
  grammar->addScope("template_string", "string.quoted.template");
  grammar->addScope("undefined", "constant.language");
  grammar->addScope("null", "constant.language.null");
  grammar->addScope("true", "constant.language.boolean.true");
  grammar->addScope("false", "constant.language.boolean.false");
  /*'comment': [
    {
      match: \"^//\",
      scopes: 'comment.line'
    },
    'comment.block'
  ]*/
  grammar->addScope("hash_bang_line", "comment.block");

  grammar->addScope(R"(
    jsx_expression > \"{\",
    jsx_expression > \"}\",
    template_substitution > \"${\",
    template_substitution > \"}\"
  )", "punctuation.section.embedded");
  grammar->addScope("template_substitution", "embedded.source");

  grammar->addScope("\"(\"", "punctuation.definition.parameters.begin.bracket.round");
  grammar->addScope("\")\"", "punctuation.definition.parameters.end.bracket.round");
  grammar->addScope("\"{\"", "punctuation.definition.function.body.begin.bracket.curly");
  grammar->addScope("\"}\"", "punctuation.definition.function.body.end.bracket.curly");
  grammar->addScope("\";\"", "punctuation.terminator.statement.semicolon");
  grammar->addScope("\"[\"", "punctuation.definition.array.begin.bracket.square");
  grammar->addScope("\"]\"", "punctuation.definition.array.end.bracket.square");

  grammar->addScope("\"var\"", "storage.type");
  grammar->addScope("\"let\"", "storage.type");
  grammar->addScope("\"class\"", "storage.type");
  grammar->addScope("\"extends\"", "storage.modifier");
  grammar->addScope("\"const\"", "storage.modifier");
  grammar->addScope("\"static\"", "storage.modifier");
  grammar->addScope("\"function\"", "storage.type.function");
  grammar->addScope("\"=>\"", "storage.type.function.arrow");

  grammar->addScope("\"=\"", "keyword.operator.js");
  grammar->addScope("\"+=\"", "keyword.operator.js");
  grammar->addScope("\"-=\"", "keyword.operator.js");
  grammar->addScope("\"*=\"", "keyword.operator.js");
  grammar->addScope("\"/=\"", "keyword.operator.js");
  grammar->addScope("\"%=\"", "keyword.operator.js");
  grammar->addScope("\"<<=\"", "keyword.operator.js");
  grammar->addScope("\">>=\"", "keyword.operator.js");
  grammar->addScope("\">>>=\"", "keyword.operator.js");
  grammar->addScope("\"&=\"", "keyword.operator.js");
  grammar->addScope("\"^=\"", "keyword.operator.js");
  grammar->addScope("\"|=\"", "keyword.operator.js");
  grammar->addScope("\"!\"", "keyword.operator.js");
  grammar->addScope("\"+\"", "keyword.operator.js");
  grammar->addScope("\"-\"", "keyword.operator.js");
  grammar->addScope("\"*\"", "keyword.operator.js");
  grammar->addScope("\"/\"", "keyword.operator.js");
  grammar->addScope("\"%\"", "keyword.operator.js");
  grammar->addScope("\"==\"", "keyword.operator.js");
  grammar->addScope("\"===\"", "keyword.operator.js");
  grammar->addScope("\"!=\"", "keyword.operator.js");
  grammar->addScope("\"!==\"", "keyword.operator.js");
  grammar->addScope("\">=\"", "keyword.operator.js");
  grammar->addScope("\"<=\"", "keyword.operator.js");
  grammar->addScope("\">\"", "keyword.operator.js");
  grammar->addScope("\"<\"", "keyword.operator.js");
  grammar->addScope("\":\"", "keyword.operator.js");
  grammar->addScope("\"?\"", "keyword.operator.js");
  grammar->addScope("\"&&\"", "keyword.operator.js");
  grammar->addScope("\"||\"", "keyword.operator.js");
  grammar->addScope("\"&\"", "keyword.operator.js");
  grammar->addScope("\"~\"", "keyword.operator.js");
  grammar->addScope("\"^\"", "keyword.operator.js");
  grammar->addScope("\">>\"", "keyword.operator.js");
  grammar->addScope("\">>>\"", "keyword.operator.js");
  grammar->addScope("\"<<\"", "keyword.operator.js");
  grammar->addScope("\"|\"", "keyword.operator.js");
  grammar->addScope("\"++\"", "keyword.operator.js");
  grammar->addScope("\"--\"", "keyword.operator.js");
  grammar->addScope("\"...\"", "keyword.operator.spread.js");

  grammar->addScope("\"in\"", "keyword.operator.in");
  grammar->addScope("\"instanceof\"", "keyword.operator.instanceof");
  grammar->addScope("\"of\"", "keyword.operator.of");
  grammar->addScope("\"new\"", "keyword.operator.new");
  grammar->addScope("\"typeof\"", "keyword.operator.typeof");

  grammar->addScope("\"get\"", "keyword.operator.setter");
  grammar->addScope("\"set\"", "keyword.operator.setter");

  grammar->addScope("\".\"", "meta.delimiter.period");
  grammar->addScope("\",\"", "meta.delimiter.comma");

  grammar->addScope("\"as\"", "keyword.control");
  grammar->addScope("\"if\"", "keyword.control");
  grammar->addScope("\"do\"", "keyword.control");
  grammar->addScope("\"else\"", "keyword.control");
  grammar->addScope("\"while\"", "keyword.control");
  grammar->addScope("\"for\"", "keyword.control");
  grammar->addScope("\"return\"", "keyword.control");
  grammar->addScope("\"break\"", "keyword.control");
  grammar->addScope("\"continue\"", "keyword.control");
  grammar->addScope("\"throw\"", "keyword.control");
  grammar->addScope("\"try\"", "keyword.control");
  grammar->addScope("\"catch\"", "keyword.control");
  grammar->addScope("\"finally\"", "keyword.control");
  grammar->addScope("\"switch\"", "keyword.control");
  grammar->addScope("\"case\"", "keyword.control");
  grammar->addScope("\"default\"", "keyword.control");
  grammar->addScope("\"export\"", "keyword.control");
  grammar->addScope("\"import\"", "keyword.control");
  grammar->addScope("\"from\"", "keyword.control");
  grammar->addScope("\"yield\"", "keyword.control");
  grammar->addScope("\"async\"", "keyword.control");
  grammar->addScope("\"await\"", "keyword.control");
  grammar->addScope("\"debugger\"", "keyword.control");
  grammar->addScope("\"delete\"", "keyword.control");

  grammar->addScope("jsx_attribute > property_identifier", "entity.other.attribute-name");
  grammar->addScope("jsx_opening_element > identifier", "entity.name.tag");
  grammar->addScope("jsx_closing_element > identifier", "entity.name.tag");
  grammar->addScope("jsx_self_closing_element > identifier", "entity.name.tag");

  return grammar;
}
