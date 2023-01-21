#include "tree-sitter-grammar.h"
#include "syntax-scope-map.h"

TreeSitterGrammar::TreeSitterGrammar(const TSLanguage *languageModule) {
  this->scopeMap = new SyntaxScopeMap();
  this->languageModule = languageModule;
  this->nextScopeId = 256 + 1;
}

TreeSitterGrammar::~TreeSitterGrammar() {
  delete this->scopeMap;
}

void TreeSitterGrammar::addScope(const std::string &selector, const std::string &result) {
  size_t start = 0;
  for (size_t i = 0; i < selector.size(); i++) {
    if (selector[i] == ',') {
      this->scopeMap->addSelector(selector.substr(start, i - start), result);
      start = i + 1;
    }
  }
  this->scopeMap->addSelector(selector.substr(start), result);
}

optional<int32_t> TreeSitterGrammar::idForScope(const optional<std::string> &scopeName) {
  if (!scopeName) {
    return optional<int32_t>();
  }
  auto id = this->idsByScope.find(*scopeName);
  if (id == this->idsByScope.end()) {
    std::string className("syntax--");
    for (char c : *scopeName) {
      if (c == '.') {
        className += " syntax--";
      } else {
        className += c;
      }
    }
    id = this->idsByScope.emplace(*scopeName, this->nextScopeId += 2).first;
    this->classNamesById.emplace(id->second, className);
    this->scopeNamesById.emplace(id->second, *scopeName);
  }
  return id->second;
}

std::string TreeSitterGrammar::classNameForScopeId(int32_t id) {
  return this->classNamesById[id];
}
