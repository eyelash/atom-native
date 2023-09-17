#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_css();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_css() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "CSS",
    "source.css",
    tree_sitter_css()
  );

  grammar->setFileTypes(
    "css"
  );

  grammar->setInjectionRegex(u"(css|CSS)");

  grammar->setScopes(
    scope("stylesheet", "source.css"),
    scope("comment", "comment"),

    scope("tag_name", "entity.name.tag"),
    scope(array("nesting_selector", "universal_selector"), "entity.name.tag"),
    scope("sibling_selector > \"~\"", "keyword.operator.combinator"),
    scope("child_selector > \">\"", "keyword.operator.combinator"),
    scope("adjacent_sibling_selector > \"+\"", "keyword.operator.combinator"),
    scope("attribute_selector > \"=\"", "keyword.operator.pattern"),
    scope("attribute_selector > \"^=\"", "keyword.operator.pattern"),
    scope("attribute_selector > \"|=\"", "keyword.operator.pattern"),
    scope("attribute_selector > \"~=\"", "keyword.operator.pattern"),
    scope("attribute_selector > \"$=\"", "keyword.operator.pattern"),
    scope("attribute_selector > \"*=\"", "keyword.operator.pattern"),
    scope("attribute_selector > plain_value", "string.unquoted.attribute-value"),
    scope("pseudo_element_selector > tag_name", "entity.other.attribute-name.pseudo-element"),
    scope("pseudo_class_selector > class_name", "entity.other.attribute-name.pseudo-class"),
    scope("class_name", "entity.other.attribute-name.class"),
    scope("id_name", "entity.other.attribute-name.id"),
    scope("namespace_name", "entity.namespace.name"),
    scope("function_name", "support.function"),

    scope(array("property_name", "plain_value"),
      match(u"^--", "variable.css")
    ),

    scope("property_name", "support.property-name"),
    scope("attribute_name", "entity.other.attribute-name"),

    scope(array(
      "\"@media\"",
      "\"@import\"",
      "\"@charset\"",
      "\"@namespace\"",
      "\"@supports\"",
      "\"@keyframes\"",
      "at_keyword"
    ), "keyword.control.at-rule"),

    scope(array("to", "from"), "keyword.control"),

    scope("important", "keyword.other.important.css"),

    scope("string_value", "string"),
    scope("color_value", "constant.other.color"),
    scope("integer_value", "numeric.constant"),
    scope("integer_value > unit", "keyword.other.unit"),
    scope("float_value", "numeric.constant"),
    scope("float_value > unit", "keyword.other.unit"),
    scope("plain_value", array(
      match(u"^(aqua|black|blue|fuchsia|gray|green|lime|maroon|navy|olive|orange|purple|red|silver|teal|white|yellow)$", "support.constant.color.w3c-standard"),
      match(u"^(aliceblue|antiquewhite|aquamarine|azure|beige|bisque|blanchedalmond|blueviolet|brown|burlywood"
      u"|cadetblue|chartreuse|chocolate|coral|cornflowerblue|cornsilk|crimson|cyan|darkblue|darkcyan"
      u"|darkgoldenrod|darkgray|darkgreen|darkgrey|darkkhaki|darkmagenta|darkolivegreen|darkorange"
      u"|darkorchid|darkred|darksalmon|darkseagreen|darkslateblue|darkslategray|darkslategrey|darkturquoise"
      u"|darkviolet|deeppink|deepskyblue|dimgray|dimgrey|dodgerblue|firebrick|floralwhite|forestgreen"
      u"|gainsboro|ghostwhite|gold|goldenrod|greenyellow|grey|honeydew|hotpink|indianred|indigo|ivory|khaki"
      u"|lavender|lavenderblush|lawngreen|lemonchiffon|lightblue|lightcoral|lightcyan|lightgoldenrodyellow"
      u"|lightgray|lightgreen|lightgrey|lightpink|lightsalmon|lightseagreen|lightskyblue|lightslategray"
      u"|lightslategrey|lightsteelblue|lightyellow|limegreen|linen|magenta|mediumaquamarine|mediumblue"
      u"|mediumorchid|mediumpurple|mediumseagreen|mediumslateblue|mediumspringgreen|mediumturquoise"
      u"|mediumvioletred|midnightblue|mintcream|mistyrose|moccasin|navajowhite|oldlace|olivedrab|orangered"
      u"|orchid|palegoldenrod|palegreen|paleturquoise|palevioletred|papayawhip|peachpuff|peru|pink|plum"
      u"|powderblue|rebeccapurple|rosybrown|royalblue|saddlebrown|salmon|sandybrown|seagreen|seashell"
      u"|sienna|skyblue|slateblue|slategray|slategrey|snow|springgreen|steelblue|tan|thistle|tomato"
      u"|transparent|turquoise|violet|wheat|whitesmoke|yellowgreen)$", "support.constant.color.w3c-extended"),
      "support.constant.property-value.css"
    )),

    scope("feature_name", "support.type.property-name"),

    scope("color_value > \"#\"", "punctuation.definition.constant.css"),
    scope("id_selector > \"#\"", "punctuation.definition.entity.css"),
    scope("selectors > \",\"", "punctuation.separator.list.comma.css"),

    scope(array("\"and\"", "\"or\"", "\"not\"", "\"only\""), "keyword.operator"),
    scope("keyword_query", "keyword.operator"),
    scope("binary_expression > \"+\"", "keyword.operator"),
    scope("binary_expression > \"-\"", "keyword.operator"),
    scope("binary_expression > \"/\"", "keyword.operator"),
    scope("binary_expression > \"*\"", "keyword.operator")
  );

  return grammar;
}
