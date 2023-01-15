#ifndef TREE_SITTER_LANGUAGE_MODE_H_
#define TREE_SITTER_LANGUAGE_MODE_H_

#include <language-mode.h>
#include <tree.h>
#include <tree-cursor.h>
#include <event-kit.h>

class TreeSitterGrammar;
class TextBuffer;

class TreeSitterLanguageMode : public LanguageMode {
public:
  class NullLayerHighlightIterator {
  public:
    NullLayerHighlightIterator();
    virtual ~NullLayerHighlightIterator();
    virtual bool seek(double, std::vector<int32_t> &, std::vector<double> &);
    virtual bool moveToSuccessor();
    virtual Point getPosition();
    virtual double compare(const NullLayerHighlightIterator *);
    virtual std::vector<int32_t> getCloseScopeIds();
    virtual std::vector<int32_t> getOpenScopeIds();
  };

  class LanguageLayer {
  public:
    TreeSitterLanguageMode *languageMode;
    TreeSitterGrammar *grammar;
    Tree tree;
    double depth;
    LanguageLayer(TreeSitterLanguageMode *, TreeSitterGrammar *, double);
    ~LanguageLayer();
    std::unique_ptr<NullLayerHighlightIterator> buildHighlightIterator();
    void handleTextChange(const TSInputEdit &, const std::u16string &, const std::u16string &);
    void update();
    void performUpdate_();
    TSInputEdit treeEditForBufferChange_(NativePoint, NativePoint, NativePoint, const std::u16string &, const std::u16string &);
  };

  class LayerHighlightIterator : public NullLayerHighlightIterator {
    LanguageLayer *languageLayer;
    double depth;
    bool atEnd;
    TreeCursor treeCursor;
    double offset;
    std::vector<std::string> containingNodeTypes;
    std::vector<double> containingNodeChildIndices;
    std::vector<double> containingNodeEndIndices;
    std::vector<int32_t> closeTags;
    std::vector<int32_t> openTags;
  public:
    LayerHighlightIterator(LanguageLayer *, TreeCursor);
    ~LayerHighlightIterator();
    bool seek(double, std::vector<int32_t> &, std::vector<double> &) override;
    bool moveToSuccessor() override;
    Point getPosition() override;
    double compare(const NullLayerHighlightIterator *) override;
    std::vector<int32_t> getCloseScopeIds() override;
    std::vector<int32_t> getOpenScopeIds() override;
    bool moveUp_(bool);
    bool moveDown_();
    bool moveRight_();
    optional<int32_t> currentScopeId_();
  };

  class HighlightIterator : public LanguageMode::HighlightIterator {
    TreeSitterLanguageMode *languageMode;
    std::vector<std::unique_ptr<NullLayerHighlightIterator>> iterators;
    bool currentScopeIsCovered;
  public:
    HighlightIterator(TreeSitterLanguageMode *);
    ~HighlightIterator();
    std::vector<int32_t> seek(Point, double) override;
    bool moveToSuccessor() override;
    void detectCoveredScope();
    Point getPosition() override;
    std::vector<int32_t> getCloseScopeIds() override;
    std::vector<int32_t> getOpenScopeIds() override;
  };

  TextBuffer *buffer;
  TreeSitterGrammar *grammar;
  LanguageLayer *rootLanguageLayer;
  Emitter<Range> didChangeHighlightingEmitter;

  TreeSitterLanguageMode(TextBuffer *, TreeSitterGrammar *);
  ~TreeSitterLanguageMode();

  void bufferDidChange(Range, Range, const std::u16string &, const std::u16string &) override;
  void bufferDidFinishTransaction() override;
  Tree parse(const TSLanguage *, const Tree &);
  std::unique_ptr<LanguageMode::HighlightIterator> buildHighlightIterator() override;
  void onDidChangeHighlighting(std::function<void(Range)>) override;
  std::string classNameForScopeId(int32_t) override;
  void emitRangeUpdate(Range);
};

#endif // TREE_SITTER_LANGUAGE_MODE_H_
