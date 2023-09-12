#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_css();

extern "C" TreeSitterGrammar *atom_language_css() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "CSS",
    "source.css",
    tree_sitter_css()
  );

  grammar->addFileTypes(
    "css"
  );

  grammar->addScopes("stylesheet", "source.css");
  grammar->addScopes("comment", "comment");

  grammar->addScopes("tag_name", "entity.name.tag");
  grammar->addScopes({"nesting_selector", "universal_selector"}, "entity.name.tag");
  grammar->addScopes("sibling_selector > \"~\"", "keyword.operator.combinator");
  grammar->addScopes("child_selector > \">\"", "keyword.operator.combinator");
  grammar->addScopes("adjacent_sibling_selector > \"+\"", "keyword.operator.combinator");
  grammar->addScopes("attribute_selector > \"=\"", "keyword.operator.pattern");
  grammar->addScopes("attribute_selector > \"^=\"", "keyword.operator.pattern");
  grammar->addScopes("attribute_selector > \"|=\"", "keyword.operator.pattern");
  grammar->addScopes("attribute_selector > \"~=\"", "keyword.operator.pattern");
  grammar->addScopes("attribute_selector > \"$=\"", "keyword.operator.pattern");
  grammar->addScopes("attribute_selector > \"*=\"", "keyword.operator.pattern");
  grammar->addScopes("attribute_selector > plain_value", "string.unquoted.attribute-value");
  grammar->addScopes("pseudo_element_selector > tag_name", "entity.other.attribute-name.pseudo-element");
  grammar->addScopes("pseudo_class_selector > class_name", "entity.other.attribute-name.pseudo-class");
  grammar->addScopes("class_name", "entity.other.attribute-name.class");
  grammar->addScopes("id_name", "entity.other.attribute-name.id");
  grammar->addScopes("namespace_name", "entity.namespace.name");
  grammar->addScopes("function_name", "support.function");

  grammar->addScopes({"property_name", "plain_value"},
    match(u"^--", "variable.css")
  );

  grammar->addScopes("property_name", "support.property-name");
  grammar->addScopes("attribute_name", "entity.other.attribute-name");

  grammar->addScopes({
    "\"@media\"",
    "\"@import\"",
    "\"@charset\"",
    "\"@namespace\"",
    "\"@supports\"",
    "\"@keyframes\"",
    "at_keyword"
  }, "keyword.control.at-rule");

  grammar->addScopes({"to", "from"}, "keyword.control");

  grammar->addScopes("important", "keyword.other.important.css");

  grammar->addScopes("string_value", "string");
  grammar->addScopes("color_value", "constant.other.color");
  grammar->addScopes("integer_value", "numeric.constant");
  grammar->addScopes("integer_value > unit", "keyword.other.unit");
  grammar->addScopes("float_value", "numeric.constant");
  grammar->addScopes("float_value > unit", "keyword.other.unit");
  grammar->addScopes("plain_value", array(
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
  ));

  grammar->addScopes("feature_name", "support.type.property-name");

  grammar->addScopes("color_value > \"#\"", "punctuation.definition.constant.css");
  grammar->addScopes("id_selector > \"#\"", "punctuation.definition.entity.css");
  grammar->addScopes("selectors > \",\"", "punctuation.separator.list.comma.css");

  grammar->addScopes({"\"and\"", "\"or\"", "\"not\"", "\"only\""}, "keyword.operator");
  grammar->addScopes("keyword_query", "keyword.operator");
  grammar->addScopes("binary_expression > \"+\"", "keyword.operator");
  grammar->addScopes("binary_expression > \"-\"", "keyword.operator");
  grammar->addScopes("binary_expression > \"/\"", "keyword.operator");
  grammar->addScopes("binary_expression > \"*\"", "keyword.operator");

  return grammar->finalize();
}
