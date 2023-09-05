#include "match-manager.h"

static const std::vector<const char16_t *> autocompleteCharacters = {
  u"()",
  u"[]",
  u"{}",
  u"\"\"",
  u"''",
  u"``",
  u"“”",
  u"‘’",
  u"«»",
  u"‹›"
};

static const std::vector<const char16_t *> pairsWithExtraNewline = {
  u"()",
  u"[]",
  u"{}"
};

void MatchManager::appendPair(std::unordered_map<char16_t, char16_t> &pairList, std::pair<char16_t, char16_t> item) {
  pairList.insert(item);
}

static std::pair<char16_t, char16_t> split(const char16_t *s) {
  return {s[0], s[1]};
}

void MatchManager::processAutoPairs(const std::vector<const char16_t *> &autocompletePairs, std::unordered_map<char16_t, char16_t> &pairedList, std::function<std::pair<char16_t, char16_t>(std::pair<char16_t, char16_t>)> dataFun) {
  if (autocompletePairs.size()) {
    for (const char16_t *autocompletePair : autocompletePairs) {
      const auto pairArray = split(autocompletePair);
      this->appendPair(pairedList, dataFun(pairArray));
    }
  }
}

void MatchManager::updateConfig() {
  using pair = std::pair<char16_t, char16_t>;
  this->processAutoPairs(::autocompleteCharacters, this->pairedCharacters, [](pair x) -> pair { return {x.first, x.second}; });
  this->processAutoPairs(::autocompleteCharacters, this->pairedCharactersInverse, [](pair x) -> pair { return {x.second, x.first}; });
  this->processAutoPairs(::pairsWithExtraNewline, this->pairsWithExtraNewline, [](pair x) -> pair { return {x.first, x.second}; });
  /*for (let startPair in this.pairedCharacters) {
    const endPair = this.pairedCharacters[startPair]
    this.pairRegexes[startPair] = new RegExp(`[${_.escapeRegExp(startPair + endPair)}]`, 'g')
  }*/
}

MatchManager::MatchManager(TextEditor *editor) {
  this->editor = editor;

  this->updateConfig();

  this->changeBracketsMode = false;
}
