#include "tree-sitter-grammar.h"
#include "syntax-scope-map.h"
#include "tree-sitter-language-mode.h"
#include <regex.h>
#include <tree-sitter.h>
#include <text-buffer.h>

TreeSitterGrammar::TreeSitterGrammar(const char *name, const char *scopeName, const TSLanguage *languageModule) : Grammar(name, scopeName) {
  this->scopeMap = new SyntaxScopeMap();
  this->languageModule = languageModule;
  this->nextScopeId = 256 + 1;
}

TreeSitterGrammar::~TreeSitterGrammar() {
  delete this->scopeMap;
}

void TreeSitterGrammar::setInjectionRegex(const char16_t *pattern) {
  this->injectionRegex = Regex(pattern);
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

std::shared_ptr<SyntaxScopeMap::Result> TreeSitterGrammar::preprocessScopes(const char *value) {
  struct StringResult final : SyntaxScopeMap::Result {
    std::string rules;
    StringResult(const char *value) : rules(value) {}
    optional<std::string> applyLeafRules(TextBuffer *, TSTreeCursor *) override {
      return rules;
    }
  };
  return std::make_shared<StringResult>(value);
}

std::shared_ptr<SyntaxScopeMap::Result> TreeSitterGrammar::preprocessScopesExact(const char16_t *exact, std::shared_ptr<SyntaxScopeMap::Result> &&scopes) {
  struct ExactResult final : SyntaxScopeMap::Result {
    std::u16string exact;
    std::shared_ptr<SyntaxScopeMap::Result> scopes;
    ExactResult(const char16_t *exact, std::shared_ptr<SyntaxScopeMap::Result> &&scopes) : exact(exact), scopes(std::move(scopes)) {}
    optional<std::string> applyLeafRules(TextBuffer *buffer, TSTreeCursor *cursor) override {
      const std::u16string nodeText = buffer->buffer->text_in_range({startPosition(cursor), endPosition(cursor)});
      return nodeText == exact
        ? scopes->applyLeafRules(buffer, cursor)
        : optional<std::string>();
    }
  };
  return std::make_shared<ExactResult>(exact, std::move(scopes));
}

std::shared_ptr<SyntaxScopeMap::Result> TreeSitterGrammar::preprocessScopesMatch(const char16_t *match, std::shared_ptr<SyntaxScopeMap::Result> &&scopes) {
  struct MatchResult final : SyntaxScopeMap::Result {
    Regex match;
    std::shared_ptr<SyntaxScopeMap::Result> scopes;
    MatchResult(const char16_t *match, std::shared_ptr<SyntaxScopeMap::Result> &&scopes) : match(match), scopes(std::move(scopes)) {}
    optional<std::string> applyLeafRules(TextBuffer *buffer, TSTreeCursor *cursor) override {
      const std::u16string nodeText = buffer->buffer->text_in_range({startPosition(cursor), endPosition(cursor)});
      return match.match(nodeText)
        ? scopes->applyLeafRules(buffer, cursor)
        : optional<std::string>();
    }
  };
  return std::make_shared<MatchResult>(match, std::move(scopes));
}

std::shared_ptr<SyntaxScopeMap::Result> TreeSitterGrammar::preprocessScopes(std::vector<std::shared_ptr<SyntaxScopeMap::Result>> &&value) {
  struct ArrayResult final : SyntaxScopeMap::Result {
    std::vector<std::shared_ptr<SyntaxScopeMap::Result>> rules;
    ArrayResult(std::vector<std::shared_ptr<SyntaxScopeMap::Result>> &&value) : rules(std::move(value)) {}
    optional<std::string> applyLeafRules(TextBuffer *buffer, TSTreeCursor *cursor) override {
      for (size_t i = 0, length = rules.size(); i != length; ++i) {
        const auto result = rules[i]->applyLeafRules(buffer, cursor);
        if (result) return result;
      }
      return optional<std::string>();
    }
  };
  return std::make_shared<ArrayResult>(std::move(value));
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

void TreeSitterGrammar::addInjectionPoint(const InjectionPoint &injectionPoint) {
  std::vector<InjectionPoint> &injectionPoints = this->injectionPointsByType[injectionPoint.type];
  injectionPoints.push_back(injectionPoint);
}

LanguageMode *TreeSitterGrammar::getLanguageMode(TextBuffer *buffer, GrammarRegistry *grammars) {
  return new TreeSitterLanguageMode(buffer, this, grammars);
}
