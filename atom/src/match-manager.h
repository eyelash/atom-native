#ifndef MATCH_MANAGER_H_
#define MATCH_MANAGER_H_

#include <unordered_map>
#include <utility>
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

#endif // MATCH_MANAGER_H_
