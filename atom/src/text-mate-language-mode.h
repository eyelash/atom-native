#ifndef TEXT_MATE_LANGUAGE_MODE_H_
#define TEXT_MATE_LANGUAGE_MODE_H_

#include <language-mode.h>

struct TextMateLanguageMode : LanguageMode {
  Grammar *grammar;

  TextMateLanguageMode(Grammar *grammar);
  ~TextMateLanguageMode();

  Grammar *getGrammar() override;
};

#endif // TEXT_MATE_LANGUAGE_MODE_H_
