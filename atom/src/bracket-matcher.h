#ifndef BRACKET_MATCHER_H_
#define BRACKET_MATCHER_H_

#include <unordered_map>
#include <utility>
#include <string>
#include <vector>
#include <functional>

struct TextEditor;

struct MatchManager {
  TextEditor *editor;
  std::unordered_map<char16_t, char16_t> pairedCharacters;
  std::unordered_map<char16_t, char16_t> pairedCharactersInverse;
  std::unordered_map<char16_t, char16_t> pairsWithExtraNewline;
  bool changeBracketsMode;

  void appendPair(std::unordered_map<char16_t, char16_t> &, std::pair<char16_t, char16_t>);
  void processAutoPairs(const std::vector<const char16_t *> &, std::unordered_map<char16_t, char16_t> &, std::function<std::pair<char16_t, char16_t>(std::pair<char16_t, char16_t>)>);
  void updateConfig();
  MatchManager(TextEditor *);
};

struct BracketMatcher {
  TextEditor *editor;
  MatchManager *matchManager;

  BracketMatcher(TextEditor *);
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
