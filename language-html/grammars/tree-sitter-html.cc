#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_html();

extern "C" TreeSitterGrammar *atom_language_html() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "HTML",
    "text.html.basic",
    tree_sitter_html()
  );

  grammar->addFileTypes(
    "html"
  );

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

  grammar->addScopes("fragment", "source.html");
  grammar->addScopes("tag_name", "entity.name.tag");
  grammar->addScopes("erroneous_end_tag_name", "invalid.illegal");
  grammar->addScopes("doctype", "meta.tag.doctype.html");
  grammar->addScopes("attribute_name", "entity.other.attribute-name");
  grammar->addScopes("attribute_value", "string.html");
  grammar->addScopes("comment", "comment.block.html");

  grammar->addScopes({
    "start_tag > \"<\"",
    "end_tag > \"</\""
  }, "punctuation.definition.tag.begin");
  grammar->addScopes({
    "start_tag > \">\"",
    "end_tag > \">\""
  }, "punctuation.definition.tag.end");

  grammar->addScopes("attribute > \"=\"", "punctuation.separator.key-value.html");

  // quoted_attribute_value has three child nodes: ", attribute_value, and ".
  // Target the first and last.
  // Single quotes and double quotes are targeted in separate selectors because
  // of quote-escaping difficulties.
  grammar->addScopes("quoted_attribute_value > \"\"\":nth-child(0)", "punctuation.definition.string.begin");
  grammar->addScopes("quoted_attribute_value > \"'\":nth-child(0)", "punctuation.definition.string.begin");
  grammar->addScopes("quoted_attribute_value > \"\"\":nth-child(2)", "punctuation.definition.string.end");
  grammar->addScopes("quoted_attribute_value > \"'\":nth-child(2)", "punctuation.definition.string.end");

  return grammar->finalize();
}
