#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_html();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_html() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "HTML",
    "text.html.basic",
    tree_sitter_html()
  );

  grammar->setFileTypes(
    "html"
  );

  grammar->setInjectionRegex(u"(HTML|html|Html)$");

  grammar->addInjectionPoint({
    "script_element",
    [](TSNode) -> std::u16string { return u"javascript"; },
    [](TSNode node) -> std::vector<TSNode> { return {ts_node_child(node, 1)}; }
  });
  grammar->addInjectionPoint({
    "style_element",
    [](TSNode) -> std::u16string { return u"css"; },
    [](TSNode node) -> std::vector<TSNode> { return {ts_node_child(node, 1)}; }
  });

  grammar->setIncreaseIndentPattern(uR"""((?x)
    <(?!\?|(?:area|base|br|col|frame|hr|html|img|input|link|meta|param)\b|[^>]*/>)
    ([-_\.A-Za-z0-9]+)(?=\s|>)\b[^>]*>(?!.*</\1>)
    |<!--(?!.*-->)
    |<\?php.+?\b(if|else(?:if)?|for(?:each)?|while)\\b.*:(?!.*end\2)
    |\{[^}"\']*$
  )""");
  grammar->setDecreaseIndentPattern(uR"""((?x)
    ^\s*
    (</(?!html)
      [-_\.A-Za-z0-9]+\b[^>]*>
      |-->
      |<\?(php)?\s+(else(if)?|end(if|for(each)?|while)|\})
      |\}
    )
  )""");

  grammar->setScopes(
    scope("fragment", "source.html"),
    scope("tag_name", "entity.name.tag"),
    scope("erroneous_end_tag_name", "invalid.illegal"),
    scope("doctype", "meta.tag.doctype.html"),
    scope("attribute_name", "entity.other.attribute-name"),
    scope("attribute_value", "string.html"),
    scope("comment", "comment.block.html"),

    scope(array(
      "start_tag > \"<\"",
      "end_tag > \"</\""
    ), "punctuation.definition.tag.begin"),
    scope(array(
      "start_tag > \">\"",
      "end_tag > \">\""
    ), "punctuation.definition.tag.end"),

    scope("attribute > \"=\"", "punctuation.separator.key-value.html"),

    // quoted_attribute_value has three child nodes: ", attribute_value, and ".
    // Target the first and last.
    // Single quotes and double quotes are targeted in separate selectors because
    // of quote-escaping difficulties.
    scope("quoted_attribute_value > \"\"\":nth-child(0)", "punctuation.definition.string.begin"),
    scope("quoted_attribute_value > \"'\":nth-child(0)", "punctuation.definition.string.begin"),
    scope("quoted_attribute_value > \"\"\":nth-child(2)", "punctuation.definition.string.end"),
    scope("quoted_attribute_value > \"'\":nth-child(2)", "punctuation.definition.string.end")
  );

  return grammar;
}
