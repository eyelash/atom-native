#include "null-grammar.h"
#include "text-mate-language-mode.h"

NullGrammar::NullGrammar() : Grammar("Null Grammar", "text.plain.null-grammar") {}

NullGrammar::~NullGrammar() {}

LanguageMode *NullGrammar::getLanguageMode(TextBuffer *, GrammarRegistry *) {
  return new TextMateLanguageMode(this);
}
