#ifndef TREE_SITTER_GRAMMAR_H_
#define TREE_SITTER_GRAMMAR_H_

#include "syntax-scope-map.h"
#include "grammar.h"
#include <regex.h>
#include <optional.h>
#include <unordered_map>

struct SyntaxScopeMap;
struct TSLanguage;

namespace TreeSitterGrammarDSL {

template <typename S, typename R> struct Scope {
  S selectors;
  R result;
  constexpr Scope(S selectors, R result) : selectors(selectors), result(result) {}
};
template <typename T> struct Exact {
  const char16_t *exact;
  T scopes;
  constexpr Exact(const char16_t *exact, T scopes) : exact(exact), scopes(scopes) {}
};
template <typename T> struct Match {
  const char16_t *match;
  T scopes;
  constexpr Match(const char16_t *match, T scopes) : match(match), scopes(scopes) {}
};
template <typename...> struct Array;
template <> struct Array<> {
  constexpr Array() {}
};
template <typename T0, typename... T> struct Array<T0, T...> {
  T0 t0;
  Array<T...> t;
  constexpr Array(T0 t0, T... t) : t0(t0), t(t...) {}
};

template <typename S, typename R> constexpr Scope<S, R> scope(S selectors, R result) {
  return Scope<S, R>(selectors, result);
}
template <typename... T> constexpr Array<T...> array(T... t) {
  return Array<T...>(t...);
}
template <typename T> constexpr Exact<T> exact(const char16_t *exact, T scopes) {
  return Exact<T>(exact, scopes);
}
template <typename T> constexpr Match<T> match(const char16_t *match, T scopes) {
  return Match<T>(match, scopes);
}

}

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
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(const char *);
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopesExact(const char16_t *, std::shared_ptr<SyntaxScopeMap::Result> &&);
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopesMatch(const char16_t *, std::shared_ptr<SyntaxScopeMap::Result> &&);
  static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(std::vector<std::shared_ptr<SyntaxScopeMap::Result>> &&);
  template <typename T> static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(const TreeSitterGrammarDSL::Exact<T> &value) {
    return preprocessScopesExact(value.exact, preprocessScopes(value.scopes));
  }
  template <typename T> static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(const TreeSitterGrammarDSL::Match<T> &value) {
    return preprocessScopesMatch(value.match, preprocessScopes(value.scopes));
  }
  static void preprocessScopes(const TreeSitterGrammarDSL::Array<> &, std::vector<std::shared_ptr<SyntaxScopeMap::Result>> &) {}
  template <typename T0, typename... T> static void preprocessScopes(const TreeSitterGrammarDSL::Array<T0, T...> &value, std::vector<std::shared_ptr<SyntaxScopeMap::Result>> &result) {
    result.push_back(preprocessScopes(value.t0));
    preprocessScopes(value.t, result);
  }
  template <typename... T> static std::shared_ptr<SyntaxScopeMap::Result> preprocessScopes(const TreeSitterGrammarDSL::Array<T...> &value) {
    std::vector<std::shared_ptr<SyntaxScopeMap::Result>> result;
    preprocessScopes(value, result);
    return preprocessScopes(std::move(result));
  }
  void addScope(const char *selector, std::shared_ptr<SyntaxScopeMap::Result> result) {
    scopeMap->addSelector(selector, result);
  }
  template <typename R> void addScope(const char *selector, const R &result) {
    addScope(selector, preprocessScopes(result));
  }
  template <typename R> void addScope(const TreeSitterGrammarDSL::Array<> &selector, const R &result) {}
  template <typename... S, typename R> void addScope(const TreeSitterGrammarDSL::Array<const char *, S...> &selector, const R &result) {
    addScope(selector.t0, result);
    addScope(selector.t, result);
  }
  template <typename S, typename R> void addScope(const TreeSitterGrammarDSL::Scope<S, R> &scope) {
    addScope(scope.selectors, scope.result);
  }
  void addScopes() {}
  template <typename T0, typename... T> void addScopes(const T0& scope, const T&... scopes) {
    addScope(scope);
    addScopes(scopes...);
  }
  template <typename... T> void setScopes(const T&... scopes) {
    addScopes(scopes...);
    scopeMap->finalize();
  }
  optional<int32_t> idForScope(const optional<std::string> &);
  std::string classNameForScopeId(int32_t);
  LanguageMode *getLanguageMode(TextBuffer *) override;
};

#endif // TREE_SITTER_GRAMMAR_H_
