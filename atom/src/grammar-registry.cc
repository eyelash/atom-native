#include "grammar-registry.h"
#include "null-grammar.h"
#include "tree-sitter-grammar.h"
#include "tree-sitter-language-mode.h"
#include <text-buffer.h>

static std::u16string getGrammarSelectionContent(TextBuffer *);

GrammarRegistry::GrammarRegistry() {
  this->nullGrammar = new NullGrammar();
}

GrammarRegistry::~GrammarRegistry() {
  delete this->nullGrammar;
  for (auto &grammar : this->treeSitterGrammarsById) {
    delete grammar.second;
  }
}

void GrammarRegistry::maintainLanguageMode(TextBuffer *buffer) {
  //this.grammarScoresByBuffer.set(buffer, null);

  //const languageOverride = this.languageOverridesByBufferId.get(buffer.id);
  //if (languageOverride) {
  //  this.assignLanguageMode(buffer, languageOverride);
  //} else {
    this->autoAssignLanguageMode(buffer);
  //}

  buffer->onDidChangePath([this, buffer]() {
    //this.grammarScoresByBuffer.delete(buffer);
    //if (!this.languageOverridesByBufferId.has(buffer.id)) {
      this->autoAssignLanguageMode(buffer);
    //}
  });
}

void GrammarRegistry::autoAssignLanguageMode(TextBuffer *buffer) {
  const auto result = this->selectGrammarWithScore(
    buffer->getPath(),
    getGrammarSelectionContent(buffer)
  );
  //this.languageOverridesByBufferId.delete(buffer.id);
  //this.grammarScoresByBuffer.set(buffer, result.score);
  if (result.first != buffer->getLanguageMode()->getGrammar()) {
    buffer->setLanguageMode(
      this->languageModeForGrammarAndBuffer(result.first, buffer)
    );
  }
}

LanguageMode *GrammarRegistry::languageModeForGrammarAndBuffer(Grammar *grammar, TextBuffer *buffer) {
  return grammar->getLanguageMode(buffer);
}

/*selectGrammar(filePath, fileContents) {
  return this.selectGrammarWithScore(filePath, fileContents).grammar;
}*/

std::pair<Grammar *, double> GrammarRegistry::selectGrammarWithScore(const optional<std::string> &filePath, const std::u16string &fileContents) {
  Grammar *bestMatch = nullptr;
  double highestScore = -INFINITY;
  for (Grammar *grammar : this->getGrammars()) {
    const double score = this->getGrammarScore(grammar, filePath, fileContents);
    if (score > highestScore || bestMatch == nullptr) {
      bestMatch = grammar;
      highestScore = score;
    }
  }
  return { bestMatch, highestScore };
}

double GrammarRegistry::getGrammarScore(Grammar *grammar, const optional<std::string> &filePath, const std::u16string &contents) {
  /*if (contents == null && fs.isFileSync(filePath)) {
    contents = fs.readFileSync(filePath, 'utf8');
  }*/

  // Initially identify matching grammars based on the filename and the first
  // line of the file.
  double score = this->getGrammarPathScore(grammar, filePath);
  //if (this.grammarMatchesPrefix(grammar, contents)) score += 0.5;

  // If multiple grammars match by one of the above criteria, break ties.
  if (score > 0) {
    /*const isTreeSitter = grammar instanceof TreeSitterGrammar;

    // Prefer either TextMate or Tree-sitter grammars based on the user's settings.
    if (isTreeSitter) {
      if (this.shouldUseTreeSitterParser(grammar.scopeName)) {
        score += 0.1;
      } else {
        return -Infinity;
      }
    }

    // Prefer grammars with matching content regexes. Prefer a grammar with no content regex
    // over one with a non-matching content regex.
    if (grammar.contentRegex) {
      const contentMatch = isTreeSitter
        ? grammar.contentRegex.test(contents)
        : grammar.contentRegex.testSync(contents);
      if (contentMatch) {
        score += 0.05;
      } else {
        score -= 0.05;
      }
    }

    // Prefer grammars that the user has manually installed over bundled grammars.
    if (!grammar.bundledPackage) score += 0.01;*/
  }

  return score;
}

static std::vector<std::string> splitPath(const std::string &path) {
  std::vector<std::string> result;
  size_t start = 0;
  for (size_t i = 0; i < path.size(); ++i) {
    char c = path[i];
    #ifdef WIN32
    if (c == '\\') c = '/';
    #endif
    if (c == '/' || c == '.') {
      result.push_back(path.substr(start, i - start));
      start = i + 1;
    }
  }
  result.push_back(path.substr(start));
  return result;
}

double GrammarRegistry::getGrammarPathScore(Grammar *grammar, const optional<std::string> &filePath) {
  if (!filePath) return -1;
  /*if (process.platform === 'win32') {
    filePath = filePath.replace(/\\/g, '/');
  }*/

  const auto pathComponents = splitPath(*filePath); //filePath.toLowerCase().split(PATH_SPLIT_REGEX);
  double pathScore = 0;

  /*let customFileTypes;
  if (this.config.get('core.customFileTypes')) {
    customFileTypes = this.config.get('core.customFileTypes')[
      grammar.scopeName
    ];
  }*/

  const auto &fileTypes = grammar->fileTypes;
  /*if (customFileTypes) {
    fileTypes = fileTypes.concat(customFileTypes);
  }*/

  for (size_t i = 0; i < fileTypes.size(); i++) {
    const auto &fileType = fileTypes[i];
    const auto fileTypeComponents = splitPath(fileType); //fileType.toLowerCase().split(PATH_SPLIT_REGEX);
    const size_t commonLength = std::min(pathComponents.size(), fileTypeComponents.size());
    std::vector<std::string> pathSuffix(pathComponents.end() - commonLength, pathComponents.end());
    //const pathSuffix = pathComponents.slice(-fileTypeComponents.length);
    if (pathSuffix == fileTypeComponents) {
      pathScore = std::max(pathScore, static_cast<double>(fileType.size()));
      /*if (i >= grammar.fileTypes.length) {
        pathScore += 0.5;
      }*/
    }
  }

  return pathScore;
}

/*void GrammarRegistry::forEachGrammar(std::function<void(TreeSitterGrammar *)> callback) {
  for (TreeSitterGrammar *grammar : this->getGrammars()) {
    callback(grammar);
  }
}*/

void GrammarRegistry::addGrammar(TreeSitterGrammar *grammar) {
  //const existingParams =
  //  this.treeSitterGrammarsById[grammar.scopeName] || {};
  if (grammar->scopeName)
    this->treeSitterGrammarsById[grammar->scopeName] = grammar;
  /*if (existingParams.injectionPoints) {
    for (const injectionPoint of existingParams.injectionPoints) {
      grammar.addInjectionPoint(injectionPoint);
    }
  }
  this.grammarAddedOrUpdated(grammar);
  return new Disposable(() => this.removeGrammar(grammar));*/
}

std::vector<Grammar *> GrammarRegistry::getGrammars() {
  std::vector<Grammar *> grammars;
  grammars.push_back(this->nullGrammar);
  for (auto &grammar : this->treeSitterGrammarsById) {
    if (grammar.second->scopeName) {
      grammars.push_back(grammar.second);
    }
  }
  return grammars;
}

static std::u16string getGrammarSelectionContent(TextBuffer *buffer) {
  return buffer->getTextInRange(
    Range(Point(0, 0), buffer->positionForCharacterIndex(1024))
  );
}
