#ifndef TREE_SITTER_LANGUAGE_MODE_H_
#define TREE_SITTER_LANGUAGE_MODE_H_

#include <language-mode.h>
#include <tree_sitter/api.h>
#include <event-kit.h>
#include <unordered_map>

struct TreeSitterGrammar;
struct GrammarRegistry;
struct TextBuffer;
struct MarkerLayer;
struct Marker;

struct TreeSitterLanguageMode final : LanguageMode {
  struct TreeEdit {
    double startIndex;
    double oldEndIndex;
    double newEndIndex;
    Point startPosition;
    Point oldEndPosition;
    Point newEndPosition;
    operator TSInputEdit() const;
  };

  struct NodeRangeSet {
    NodeRangeSet *previous;
    std::vector<TSNode> nodes;
    bool newlinesBetween;
    bool includeChildren;
    NodeRangeSet(NodeRangeSet *, const std::vector<TSNode> &, bool, bool);
    std::vector<TSRange> getRanges(TextBuffer *);
    void pushRange_(TextBuffer *, const std::vector<TSRange> &, std::vector<TSRange> &, TSRange);
    void ensureNewline_(TextBuffer *, std::vector<TSRange> &, double, const Point &);
  };

  struct LayerHighlightIterator;
  struct LanguageLayer {
    Marker *marker;
    TreeSitterLanguageMode *languageMode;
    TreeSitterGrammar *grammar;
    TSTree *tree;
    double depth;
    optional<Range> editedRange;
    LanguageLayer(Marker *, TreeSitterLanguageMode *, TreeSitterGrammar *, double);
    ~LanguageLayer();
    std::unique_ptr<LayerHighlightIterator> buildHighlightIterator();
    void handleTextChange(const TreeEdit &, const std::u16string &, const std::u16string &);
    void destroy();
    void update(NodeRangeSet *);
    void performUpdate_(NodeRangeSet *);
    void populateInjections_(Range, NodeRangeSet *);
    TreeEdit treeEditForBufferChange_(const Point &, const Point &, const Point &, const std::u16string &, const std::u16string &);
  };

  struct LayerHighlightIterator {
    LanguageLayer *languageLayer;
    double depth;
    bool atEnd;
    TSTreeCursor treeCursor;
    double offset;
    std::vector<std::string> containingNodeTypes;
    std::vector<double> containingNodeChildIndices;
    std::vector<double> containingNodeEndIndices;
    std::vector<int32_t> closeTags;
    std::vector<int32_t> openTags;
    LayerHighlightIterator(LanguageLayer *, TSTreeCursor);
    ~LayerHighlightIterator();
    bool seek(double, std::vector<int32_t> &, std::vector<double> &);
    bool moveToSuccessor();
    Point getPosition();
    double compare(const LayerHighlightIterator *);
    std::vector<int32_t> getCloseScopeIds();
    std::vector<int32_t> getOpenScopeIds();
    bool isAtInjectionBoundary();
    bool moveUp_(bool);
    bool moveDown_();
    bool moveRight_();
    optional<int32_t> currentScopeId_();
  };

  struct HighlightIterator final : LanguageMode::HighlightIterator {
    TreeSitterLanguageMode *languageMode;
    std::vector<std::unique_ptr<LayerHighlightIterator>> iterators;
    bool currentScopeIsCovered;
    HighlightIterator(TreeSitterLanguageMode *);
    ~HighlightIterator();
    std::vector<int32_t> seek(const Point &, double) override;
    void moveToSuccessor() override;
    void detectCoveredScope();
    Point getPosition() override;
    std::vector<int32_t> getCloseScopeIds() override;
    std::vector<int32_t> getOpenScopeIds() override;
  };

  TextBuffer *buffer;
  TreeSitterGrammar *grammar;
  GrammarRegistry *grammarRegistry;
  LanguageLayer *rootLanguageLayer;
  MarkerLayer *injectionsMarkerLayer;
  std::unordered_map<Marker *, LanguageLayer *> languageLayersByMarker;
  std::unordered_map<Marker *, LanguageLayer *> parentLanguageLayersByMarker;
  Emitter<const Range &> didChangeHighlightingEmitter;

  TreeSitterLanguageMode(TextBuffer *, TreeSitterGrammar *, GrammarRegistry *);
  ~TreeSitterLanguageMode();

  void bufferDidChange(const Range &, const Range &, const std::u16string &, const std::u16string &) override;
  void bufferDidFinishTransaction() override;
  TSTree *parse(const TSLanguage *, TSTree *, const std::vector<TSRange> &);
  std::unique_ptr<LanguageMode::HighlightIterator> buildHighlightIterator() override;
  void onDidChangeHighlighting(std::function<void(const Range &)>) override;
  std::string classNameForScopeId(int32_t) override;
  bool isRowCommented(double) override;
  double suggestedIndentForLineAtBufferRow(double, const std::u16string &, double) override;
  double suggestedIndentForBufferRow(double, double, bool) override;
  optional<double> suggestedIndentForEditedBufferRow(double, double) override;
  double suggestedIndentForLineWithScopeAtBufferRow_(double, const std::u16string &, double, bool = true);
  double indentLevelForLine(const std::u16string &, double);
  void forEachTreeWithRange_(const Range &, std::function<void(TSTree *, TreeSitterGrammar *)>);
  TSNode getSyntaxNodeContainingRange(const Range &, std::function<bool(TSNode, TreeSitterGrammar *)> = [](TSNode, TreeSitterGrammar *) { return true; });
  std::pair<TSNode, TreeSitterGrammar *> getSyntaxNodeAndGrammarContainingRange(const Range &, std::function<bool(TSNode, TreeSitterGrammar *)> = [](TSNode, TreeSitterGrammar *) { return true; });
  optional<Range> getRangeForSyntaxNodeContainingRange(const Range &);
  TSNode getSyntaxNodeAtPosition(const Point &, std::function<bool(TSNode, TreeSitterGrammar *)>);
  Grammar *getGrammar() override;
  optional<NativeRange> firstNonWhitespaceRange(double);
  TreeSitterGrammar *grammarForLanguageString(const std::u16string &);
  void emitRangeUpdate(const Range &);
};

#endif // TREE_SITTER_LANGUAGE_MODE_H_
