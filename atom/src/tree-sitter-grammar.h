#ifndef TREE_SITTER_GRAMMAR_H_
#define TREE_SITTER_GRAMMAR_H_

#include "syntax-scope-map.h"
#include "grammar.h"
#include <regex.h>
#include <optional.h>
#include <unordered_map>

struct SyntaxScopeMap;
struct TSLanguage;

struct TreeSitterGrammar final : Grammar {
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
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(std::shared_ptr<SyntaxScopeMap::Result> &&);
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(const char *);
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopesExact(const char16_t *, std::shared_ptr<SyntaxScopeMap::Result> &&);
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopesMatch(const char16_t *, std::shared_ptr<SyntaxScopeMap::Result> &&);
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(std::vector<std::shared_ptr<SyntaxScopeMap::Result>> &&);
  template <typename T0, typename T1, typename... T> static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(T0&& t0, T1&& t1, T&&... t) {
    return preprocessScopes({
      preprocessScopes(std::forward<T0>(t0)),
      preprocessScopes(std::forward<T1>(t1)),
      preprocessScopes(std::forward<T>(t))...
    });
  }
  void addScopes(const char *, std::shared_ptr<SyntaxScopeMap::Result>);
  void addScopes(std::initializer_list<const char *>, std::shared_ptr<SyntaxScopeMap::Result>);
  template <typename... T> void addScopes(const char *selector, T&&... t) {
    addScopes(selector, preprocessScopes(std::forward<T>(t)...));
  }
  template <typename... T> void addScopes(std::initializer_list<const char *> selectors, T&&... t) {
    addScopes(selectors, preprocessScopes(std::forward<T>(t)...));
  }
  TreeSitterGrammar *finalize();
  optional<int32_t> idForScope(const optional<std::string> &);
  std::string classNameForScopeId(int32_t);
  LanguageMode *getLanguageMode(TextBuffer *) override;
};
template <typename T> std::shared_ptr<SyntaxScopeMap::Result> exact(const char16_t *exact, T&& t) {
  return TreeSitterGrammar::preprocessScopesExact(exact, TreeSitterGrammar::preprocessScopes(std::forward<T>(t)));
}
template <typename T> std::shared_ptr<SyntaxScopeMap::Result> match(const char16_t *match, T&& t) {
  return TreeSitterGrammar::preprocessScopesMatch(match, TreeSitterGrammar::preprocessScopes(std::forward<T>(t)));
}
template <typename... T> std::shared_ptr<SyntaxScopeMap::Result> array(T&&... t) {
  return TreeSitterGrammar::preprocessScopes({ TreeSitterGrammar::preprocessScopes(std::forward<T>(t))... });
}

#endif // TREE_SITTER_GRAMMAR_H_
