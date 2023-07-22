#include "text-mate-language-mode.h"

TextMateLanguageMode::TextMateLanguageMode(Grammar *grammar) {
  this->grammar = grammar;
}

TextMateLanguageMode::~TextMateLanguageMode() {}

Grammar *TextMateLanguageMode::getGrammar() {
  return this->grammar;
}
