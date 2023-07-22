#include "tree-sitter-grammar.h"
#include "syntax-scope-map.h"
#include "tree-sitter-language-mode.h"
#include <regex.h>
#include <tree-cursor.h>

TreeSitterGrammar::TreeSitterGrammar(const char *name, const char *scopeName, const TSLanguage *languageModule) : Grammar(name, scopeName) {
  this->scopeMap = new SyntaxScopeMap();
  this->languageModule = languageModule;
  this->nextScopeId = 256 + 1;
}

TreeSitterGrammar::~TreeSitterGrammar() {
  delete this->scopeMap;
}

void TreeSitterGrammar::setIncreaseIndentPattern(const char16_t *pattern) {
  this->increaseIndentRegex = Regex(pattern);
}

void TreeSitterGrammar::setDecreaseIndentPattern(const char16_t *pattern) {
  this->decreaseIndentRegex = Regex(pattern);
}

void TreeSitterGrammar::setDecreaseNextIndentPattern(const char16_t *pattern) {
  this->decreaseNextIndentRegex = Regex(pattern);
}

SyntaxScopeMap::Result *TreeSitterGrammar::preprocessScopes(const char *value) {
  struct StringResult : SyntaxScopeMap::Result {
    std::string rules;
    StringResult(const char *value) : rules(value) {}
    optional<std::string> applyLeafRules(const TreeCursor &) override {
      return rules;
    }
  };
  return new StringResult(value);
}

SyntaxScopeMap::Result *TreeSitterGrammar::preprocessScopes(Exact value) {
  struct ExactResult : SyntaxScopeMap::Result {
    std::u16string exact;
    std::string scopes;
    ExactResult(Exact value) : exact(value.exact), scopes(value.scopes) {}
    optional<std::string> applyLeafRules(const TreeCursor &cursor) override {
      return cursor.nodeText() == exact
        ? scopes
        : optional<std::string>();
    }
  };
  return new ExactResult(value);
}

SyntaxScopeMap::Result *TreeSitterGrammar::preprocessScopes(Match value) {
  struct MatchResult : SyntaxScopeMap::Result {
    Regex match;
    std::string scopes;
    MatchResult(Match value) : match(value.match, nullptr), scopes(value.scopes) {}
    optional<std::string> applyLeafRules(const TreeCursor &cursor) override {
      return match.match(cursor.nodeText())
        ? scopes
        : optional<std::string>();
    }
  };
  return new MatchResult(value);
}

SyntaxScopeMap::Result *TreeSitterGrammar::preprocessScopes(std::initializer_list<SyntaxScopeMap::Result *> value) {
  struct ArrayResult : SyntaxScopeMap::Result {
    std::vector<SyntaxScopeMap::Result *> rules;
    ArrayResult(std::initializer_list<SyntaxScopeMap::Result *> value) : rules(value) {}
    ~ArrayResult() {
      for (SyntaxScopeMap::Result *result : this->rules) {
        delete result;
      }
    }
    optional<std::string> applyLeafRules(const TreeCursor &cursor) override {
      for (size_t i = 0, length = rules.size(); i != length; ++i) {
        const auto result = rules[i]->applyLeafRules(cursor);
        if (result) return result;
      }
      return optional<std::string>();
    }
  };
  return new ArrayResult(value);
}

void TreeSitterGrammar::addScopes(const char *selector, SyntaxScopeMap::Result *result) {
  this->scopeMap->addSelector(selector, result);
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

LanguageMode *TreeSitterGrammar::getLanguageMode(TextBuffer *buffer) {
  return new TreeSitterLanguageMode(buffer, this);
}
