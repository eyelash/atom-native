#ifndef BRACKET_MATCHER_H_
#define BRACKET_MATCHER_H_

#include <string>

struct TextEditor;
struct MatchManager;

struct BracketMatcher {
  TextEditor *editor;
  MatchManager *matchManager;

  BracketMatcher(TextEditor *, MatchManager *);
  ~BracketMatcher();

  void insertText(const std::u16string &, bool = false);
  void insertNewline();
  void backspace();
  bool wrapSelectionInBrackets(const std::u16string &);
  bool isQuote(const std::u16string &);
  bool isOpeningBracket(const std::u16string &);
  bool isClosingBracket(const std::u16string &);
};

#endif // BRACKET_MATCHER_H_
