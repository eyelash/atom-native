#ifndef TREE_SITTER_GRAMMAR_H_
#define TREE_SITTER_GRAMMAR_H_

#include "syntax-scope-map.h"
#include "grammar.h"
#include <regex.h>
#include <optional.h>
#include <unordered_map>

struct SyntaxScopeMap;
struct TSLanguage;

struct TreeSitterGrammar : Grammar {
  struct Exact {
    const char16_t *exact;
    const char *scopes;
  };
  struct Match {
    const char16_t *match;
    const char *scopes;
  };

  SyntaxScopeMap *scopeMap;
  const TSLanguage *languageModule;
  std::unordered_map<int32_t, std::string> classNamesById;
  std::unordered_map<int32_t, std::string> scopeNamesById;
  std::unordered_map<std::string, int32_t> idsByScope;
  int32_t nextScopeId;
  Regex increaseIndentRegex;
  Regex decreaseIndentRegex;
  Regex decreaseNextIndentRegex;

  TreeSitterGrammar(const char *, const char *, const TSLanguage *);
  ~TreeSitterGrammar();

  void setIncreaseIndentPattern(const char16_t *);
  void setDecreaseIndentPattern(const char16_t *);
  void setDecreaseNextIndentPattern(const char16_t *);
  static SyntaxScopeMap::Result *preprocessScopes(const char *);
  static SyntaxScopeMap::Result *preprocessScopes(Exact);
  static SyntaxScopeMap::Result *preprocessScopes(Match);
  static SyntaxScopeMap::Result *preprocessScopes(std::initializer_list<SyntaxScopeMap::Result *>);
  template <typename T0, typename T1, typename... T> static SyntaxScopeMap::Result *preprocessScopes(T0&& t0, T1&& t1, T&&... t) {
    return preprocessScopes({
      preprocessScopes(std::forward<T0>(t0)),
      preprocessScopes(std::forward<T1>(t1)),
      preprocessScopes(std::forward<T>(t))...
    });
  }
  void addScopes(const char *, SyntaxScopeMap::Result *);
  template <typename... T> void addScopes(const char *selector, T&&... t) {
    addScopes(selector, preprocessScopes(std::forward<T>(t)...));
  }
  template <typename... T> void addScopes(std::initializer_list<const char *> selectors, T&&... t) {
    for (const char *selector : selectors) {
      addScopes(selector, std::forward<T>(t)...);
    }
  }
  optional<int32_t> idForScope(const optional<std::string> &);
  std::string classNameForScopeId(int32_t);
};

#endif // TREE_SITTER_GRAMMAR_H_
