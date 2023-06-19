#ifndef GRAMMAR_REGISTRY_H_
#define GRAMMAR_REGISTRY_H_

#include <unordered_map>
#include <string>
#include <vector>
#include <optional.h>

struct TextBuffer;
struct LanguageMode;
struct TreeSitterGrammar;

struct GrammarRegistry {
  std::unordered_map<std::string, TreeSitterGrammar *> treeSitterGrammarsById;

  GrammarRegistry();
  ~GrammarRegistry();

  void autoAssignLanguageMode(TextBuffer *);
  LanguageMode *languageModeForGrammarAndBuffer(TreeSitterGrammar *, TextBuffer *);
  std::pair<TreeSitterGrammar *, double> selectGrammarWithScore(const optional<std::string> &, const std::u16string &);
  double getGrammarScore(TreeSitterGrammar *, const optional<std::string> &, const std::u16string &);
  double getGrammarPathScore(TreeSitterGrammar *, const optional<std::string> &);
  void addGrammar(TreeSitterGrammar *);
  std::vector<TreeSitterGrammar *> getGrammars();
};

#endif // GRAMMAR_REGISTRY_H_
