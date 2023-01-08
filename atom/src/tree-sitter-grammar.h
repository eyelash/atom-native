#ifndef TREE_SITTER_GRAMMAR_H_
#define TREE_SITTER_GRAMMAR_H_

#include <optional.h>
#include <unordered_map>
#include <string>

class SyntaxScopeMap;
struct TSLanguage;

class TreeSitterGrammar {
public:
  SyntaxScopeMap *scopeMap;
  const TSLanguage *languageModule;
  std::unordered_map<int32_t, std::string> classNamesById;
  std::unordered_map<int32_t, std::string> scopeNamesById;
  std::unordered_map<std::string, int32_t> idsByScope;
  int32_t nextScopeId;

  TreeSitterGrammar(const TSLanguage *);
  ~TreeSitterGrammar();

  optional<int32_t> idForScope(const optional<std::string> &);
  std::string classNameForScopeId(int32_t);
};

#endif // TREE_SITTER_GRAMMAR_H_
