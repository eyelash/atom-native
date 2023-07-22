#ifndef GRAMMAR_REGISTRY_H_
#define GRAMMAR_REGISTRY_H_

#include <unordered_map>
#include <string>
#include <vector>
#include <optional.h>

struct TextBuffer;
struct LanguageMode;
struct Grammar;
struct TreeSitterGrammar;

struct GrammarRegistry {
  Grammar *nullGrammar;
  std::unordered_map<std::string, TreeSitterGrammar *> treeSitterGrammarsById;

  GrammarRegistry();
  ~GrammarRegistry();

  void maintainLanguageMode(TextBuffer *);
  void autoAssignLanguageMode(TextBuffer *);
  LanguageMode *languageModeForGrammarAndBuffer(Grammar *, TextBuffer *);
  std::pair<Grammar *, double> selectGrammarWithScore(const optional<std::string> &, const std::u16string &);
  double getGrammarScore(Grammar *, const optional<std::string> &, const std::u16string &);
  double getGrammarPathScore(Grammar *, const optional<std::string> &);
  void addGrammar(TreeSitterGrammar *);
  std::vector<Grammar *> getGrammars();
};

#endif // GRAMMAR_REGISTRY_H_
