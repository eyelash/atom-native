#ifndef NULL_GRAMMAR_H_
#define NULL_GRAMMAR_H_

#include "grammar.h"

struct NullGrammar final : Grammar {
  NullGrammar();
  ~NullGrammar();

  LanguageMode *getLanguageMode(TextBuffer *) override;
};

#endif // NULL_GRAMMAR_H_
