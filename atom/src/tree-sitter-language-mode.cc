#include "tree-sitter-language-mode.h"
#include "tree-sitter-grammar.h"
#include "syntax-scope-map.h"
#include <parser.h>
#include <text-buffer.h>

static void insertContainingTag(int32_t, double, std::vector<int32_t> &, std::vector<double> &);
static TreeSitterLanguageMode::NullLayerHighlightIterator *last(const std::vector<std::unique_ptr<TreeSitterLanguageMode::NullLayerHighlightIterator>> &);

TreeSitterLanguageMode::TreeSitterLanguageMode(TextBuffer *buffer, TreeSitterGrammar *grammar) {
  this->buffer = buffer;
  this->grammar = grammar;
  this->rootLanguageLayer = new LanguageLayer(this, grammar, 0);
}

TreeSitterLanguageMode::~TreeSitterLanguageMode() {
  delete this->rootLanguageLayer;
}

void TreeSitterLanguageMode::bufferDidChange(Range oldRange, Range newRange, const std::u16string &oldText, const std::u16string &newText) {
  const auto edit = this->rootLanguageLayer->treeEditForBufferChange_(
    oldRange.start,
    oldRange.end,
    newRange.end,
    oldText,
    newText
  );
  this->rootLanguageLayer->handleTextChange(edit, oldText, newText);
  /*for (const marker of this.injectionsMarkerLayer.getMarkers()) {
    marker.languageLayer.handleTextChange(edit, oldText, newText);
  }*/
}

void TreeSitterLanguageMode::bufferDidFinishTransaction() {
  /*for (let i = 0, { length } = changes; i < length; i++) {
    const { oldRange, newRange } = changes[i];
    spliceArray(
      this.isFoldableCache,
      newRange.start.row,
      oldRange.end.row - oldRange.start.row,
      { length: newRange.end.row - newRange.start.row }
    );
  }*/
  this->rootLanguageLayer->update();
}

Tree TreeSitterLanguageMode::parse(const TSLanguage *language, const Tree &oldTree /* , ranges */) {
  Parser *parser = /* PARSER_POOL.pop() || */ new Parser();
  parser->setLanguage(language);
  const Tree result = parser->parseTextBufferSync(this->buffer->buffer, oldTree /* , {
    syncTimeoutMicros: this.syncTimeoutMicros,
    includedRanges: ranges
  } */);

  /*if (result.then) {
    return result.then(tree => {
      PARSER_POOL.push(parser);
      return tree;
    });
  } else {
    PARSER_POOL.push(parser);
    return result;
  }*/
  delete parser;
  return result;
}

/*
Section - Highlighting
*/

std::unique_ptr<LanguageMode::HighlightIterator> TreeSitterLanguageMode::buildHighlightIterator() {
  //if (!this.rootLanguageLayer) return new NullLanguageModeHighlightIterator();
  return std::unique_ptr<LanguageMode::HighlightIterator>(new HighlightIterator(this));
}

std::string TreeSitterLanguageMode::classNameForScopeId(int32_t scopeId) {
  return this->grammar->classNameForScopeId(scopeId);
}

/*
LanguageLayer
*/

TreeSitterLanguageMode::LanguageLayer::LanguageLayer(TreeSitterLanguageMode *languageMode, TreeSitterGrammar *grammar, double depth) {
  this->languageMode = languageMode;
  this->grammar = grammar;
  this->depth = depth;
}

TreeSitterLanguageMode::LanguageLayer::~LanguageLayer() {}

std::unique_ptr<TreeSitterLanguageMode::NullLayerHighlightIterator> TreeSitterLanguageMode::LanguageLayer::buildHighlightIterator() {
  if (this->tree) {
    return std::unique_ptr<TreeSitterLanguageMode::NullLayerHighlightIterator>(new LayerHighlightIterator(this, this->tree.walk()));
  } else {
    return std::unique_ptr<TreeSitterLanguageMode::NullLayerHighlightIterator>(new NullLayerHighlightIterator());
  }
}

void TreeSitterLanguageMode::LanguageLayer::handleTextChange(const TSInputEdit &edit, const std::u16string &oldText, const std::u16string &newText) {
  //const { startPosition, oldEndPosition, newEndPosition } = edit;

  if (this->tree) {
    this->tree.edit(edit);
    /*if (this.editedRange) {
      if (startPosition.isLessThan(this.editedRange.start)) {
        this.editedRange.start = startPosition;
      }
      if (oldEndPosition.isLessThan(this.editedRange.end)) {
        this.editedRange.end = newEndPosition.traverse(
          this.editedRange.end.traversalFrom(oldEndPosition)
        );
      } else {
        this.editedRange.end = newEndPosition;
      }
    } else {
      this.editedRange = new Range(startPosition, newEndPosition);
    }*/
  }

  /*if (this.patchSinceCurrentParseStarted) {
    this.patchSinceCurrentParseStarted.splice(
      startPosition,
      oldEndPosition.traversalFrom(startPosition),
      newEndPosition.traversalFrom(startPosition),
      oldText,
      newText
    );
  }*/
}

void TreeSitterLanguageMode::LanguageLayer::update(/* nodeRangeSet */) {
  this->performUpdate_();
  /*if (!this.currentParsePromise) {
    while (
      !this.destroyed &&
      (!this.tree || this.tree.rootNode.hasChanges())
    ) {
      const params = { async: false };
      this.currentParsePromise = this._performUpdate(nodeRangeSet, params);
      if (!params.async) break;
      await this.currentParsePromise;
    }
    this.currentParsePromise = null;
  }*/
}

void TreeSitterLanguageMode::LanguageLayer::performUpdate_(/* nodeRangeSet, params */) {
  //let includedRanges = null;
  /*if (nodeRangeSet) {
    includedRanges = nodeRangeSet.getRanges(this.languageMode.buffer);
    if (includedRanges.length === 0) {
      const range = this.marker.getRange();
      this.destroy();
      this.languageMode.emitRangeUpdate(range);
      return;
    }
  }*/

  //let affectedRange = this.editedRange;
  //this.editedRange = null;

  //this.patchSinceCurrentParseStarted = new Patch();
  auto tree = this->languageMode->parse(
    this->grammar->languageModule,
    this->tree
    //includedRanges
  );
  /*if (tree.then) {
    params.async = true;
    tree = await tree;
  }*/

  /*const changes = this.patchSinceCurrentParseStarted.getChanges();
  this.patchSinceCurrentParseStarted = null;
  for (const {
    oldStart,
    newStart,
    oldEnd,
    newEnd,
    oldText,
    newText
  } of changes) {
    const newExtent = Point.fromObject(newEnd).traversalFrom(newStart);
    tree.edit(
      this._treeEditForBufferChange(
        newStart,
        oldEnd,
        Point.fromObject(oldStart).traverse(newExtent),
        oldText,
        newText
      )
    );
  }*/

  if (this->tree) {
    //const rangesWithSyntaxChanges = this.tree.getChangedRanges(tree);
    this->tree = tree;

    /*if (rangesWithSyntaxChanges.length > 0) {
      for (const range of rangesWithSyntaxChanges) {
        this.languageMode.emitRangeUpdate(rangeForNode(range));
      }

      const combinedRangeWithSyntaxChange = new Range(
        rangesWithSyntaxChanges[0].startPosition,
        last(rangesWithSyntaxChanges).endPosition
      );

      if (affectedRange) {
        this.languageMode.emitRangeUpdate(affectedRange);
        affectedRange = affectedRange.union(combinedRangeWithSyntaxChange);
      } else {
        affectedRange = combinedRangeWithSyntaxChange;
      }
    }*/
  } else {
    this->tree = tree;
    /*this.languageMode.emitRangeUpdate(rangeForNode(tree.rootNode));
    if (includedRanges) {
      affectedRange = new Range(
        includedRanges[0].startPosition,
        last(includedRanges).endPosition
      );
    } else {
      affectedRange = MAX_RANGE;
    }*/
  }

  /*if (affectedRange) {
    const injectionPromise = this._populateInjections(
      affectedRange,
      nodeRangeSet
    );
    if (injectionPromise) {
      params.async = true;
      return injectionPromise;
    }
  }*/
}

TSInputEdit TreeSitterLanguageMode::LanguageLayer::treeEditForBufferChange_(NativePoint start, NativePoint oldEnd, NativePoint newEnd, const std::u16string &oldText, const std::u16string &newText) {
  const uint32_t startIndex = this->languageMode->buffer->characterIndexForPosition(
    start
  );
  return {
    startIndex,
    startIndex + static_cast<uint32_t>(oldText.size()),
    startIndex + static_cast<uint32_t>(newText.size()),
    {start.row, start.column},
    {oldEnd.row, oldEnd.column},
    {newEnd.row, newEnd.column}
  };
}

/*
HighlightIterator
*/

TreeSitterLanguageMode::HighlightIterator::HighlightIterator(TreeSitterLanguageMode *languageMode) {
  this->languageMode = languageMode;
  this->currentScopeIsCovered = false;
}

TreeSitterLanguageMode::HighlightIterator::~HighlightIterator() {}

std::vector<int32_t> TreeSitterLanguageMode::HighlightIterator::seek(Point targetPosition, double endRow) {
  /*const injectionMarkers = this.languageMode.injectionsMarkerLayer.findMarkers(
    {
      intersectsRange: new Range(targetPosition, new Point(endRow + 1, 0))
    }
  );*/

  std::vector<int32_t> containingTags;
  std::vector<double> containingTagStartIndices;
  const double targetIndex = this->languageMode->buffer->characterIndexForPosition(
    targetPosition
  );

  this->iterators.clear();
  std::unique_ptr<NullLayerHighlightIterator> iterator = this->languageMode->rootLanguageLayer->buildHighlightIterator();
  if (iterator->seek(targetIndex, containingTags, containingTagStartIndices)) {
    this->iterators.push_back(std::move(iterator));
  }

  // Populate the iterators array with all of the iterators whose syntax
  // trees span the given position.
  /*for (const marker of injectionMarkers) {
    const iterator = marker.languageLayer.buildHighlightIterator();
    if (
      iterator.seek(targetIndex, containingTags, containingTagStartIndices)
    ) {
      this.iterators.push(iterator);
    }
  }*/

  // Sort the iterators so that the last one in the array is the earliest
  // in the document, and represents the current position.
  //this.iterators.sort((a, b) => b.compare(a));
  this->detectCoveredScope();

  return containingTags;
}

bool TreeSitterLanguageMode::HighlightIterator::moveToSuccessor() {
  // Advance the earliest layer iterator to its next scope boundary.
  NullLayerHighlightIterator *leader = last(this->iterators);

  // Maintain the sorting of the iterators by their position in the document.
  if (leader->moveToSuccessor()) {
    const size_t leaderIndex = this->iterators.size() - 1;
    size_t i = leaderIndex;
    while (i > 0 && this->iterators[i - 1]->compare(leader) < 0) i--;
    if (i < leaderIndex) {
      //this->iterators.splice(i, 0, this->iterators.pop());
      std::unique_ptr<NullLayerHighlightIterator> iterator = std::move(this->iterators.back());
      this->iterators.pop_back();
      this->iterators.insert(this->iterators.begin() + i, std::move(iterator));
    }
  } else {
    // If the layer iterator was at the end of its syntax tree, then remove
    // it from the array.
    this->iterators.pop_back();
  }

  this->detectCoveredScope();
  return false;
}

void TreeSitterLanguageMode::HighlightIterator::detectCoveredScope() {
  const size_t layerCount = this->iterators.size();
  if (layerCount > 1) {
    /*LayerHighlightIterator *first = this->iterators[layerCount - 1];
    LayerHighlightIterator *next = this->iterators[layerCount - 2];
    if (
      next->offset == first->offset &&
      next->atEnd == first->atEnd &&
      next->depth > first->depth &&
      !next->isAtInjectionBoundary()
    ) {
      this->currentScopeIsCovered = true;
      return;
    }*/
  }
  this->currentScopeIsCovered = false;
}

Point TreeSitterLanguageMode::HighlightIterator::getPosition() {
  NullLayerHighlightIterator *iterator = last(this->iterators);
  if (iterator) {
    return iterator->getPosition();
  } else {
    return Point::INFINITY_;
  }
}

std::vector<int32_t> TreeSitterLanguageMode::HighlightIterator::getCloseScopeIds() {
  NullLayerHighlightIterator *iterator = last(this->iterators);
  if (iterator && !this->currentScopeIsCovered) {
    return iterator->getCloseScopeIds();
  }
  return {};
}

std::vector<int32_t> TreeSitterLanguageMode::HighlightIterator::getOpenScopeIds() {
  NullLayerHighlightIterator *iterator = last(this->iterators);
  if (iterator && !this->currentScopeIsCovered) {
    return iterator->getOpenScopeIds();
  }
  return {};
}

/*
LayerHighlightIterator
*/

TreeSitterLanguageMode::LayerHighlightIterator::LayerHighlightIterator(LanguageLayer *languageLayer, TreeCursor treeCursor): treeCursor{treeCursor} {
  this->languageLayer = languageLayer;
  this->depth = this->languageLayer->depth;

  // The iterator is always positioned at either the start or the end of some node
  // in the syntax tree.
  this->atEnd = false;
  //this->treeCursor = treeCursor;
  this->offset = 0;

  // In order to determine which selectors match its current node, the iterator maintains
  // a list of the current node's ancestors. Because the selectors can use the `:nth-child`
  // pseudo-class, each node's child index is also stored.
  //this->containingNodeTypes = {};
  //this->containingNodeChildIndices = {};
  //this->containingNodeEndIndices = {};

  // At any given position, the iterator exposes the list of class names that should be
  // *ended* at its current position and the list of class names that should be *started*
  // at its current position.
  //this->closeTags = {};
  //this->openTags = {};
}

TreeSitterLanguageMode::LayerHighlightIterator::~LayerHighlightIterator() {}

bool TreeSitterLanguageMode::LayerHighlightIterator::seek(double targetIndex, std::vector<int32_t> &containingTags, std::vector<double> &containingTagStartIndices) {
  while (this->treeCursor.gotoParent()) {}

  this->atEnd = true;
  this->closeTags.clear();
  this->openTags.clear();
  this->containingNodeTypes.clear();
  this->containingNodeChildIndices.clear();
  this->containingNodeEndIndices.clear();

  std::vector<double> containingTagEndIndices;

  if (targetIndex >= this->treeCursor.endIndex()) {
    return false;
  }

  optional<double> childIndex = -1;
  for (;;) {
    this->containingNodeTypes.push_back(this->treeCursor.nodeType());
    this->containingNodeChildIndices.push_back(*childIndex);
    this->containingNodeEndIndices.push_back(this->treeCursor.endIndex());

    const auto scopeId = this->currentScopeId_();
    if (scopeId) {
      if (this->treeCursor.startIndex() < targetIndex) {
        insertContainingTag(
          *scopeId,
          this->treeCursor.startIndex(),
          containingTags,
          containingTagStartIndices
        );
        containingTagEndIndices.push_back(this->treeCursor.endIndex());
      } else {
        this->atEnd = false;
        this->openTags.push_back(*scopeId);
        this->moveDown_();
        break;
      }
    }

    childIndex = this->treeCursor.gotoFirstChildForIndex(targetIndex);
    if (!childIndex) break;
    if (this->treeCursor.startIndex() >= targetIndex) this->atEnd = false;
  }

  if (this->atEnd) {
    this->offset = this->treeCursor.endIndex();
    for (size_t i = 0, length = containingTags.size(); i < length; i++) {
      if (i < containingTagEndIndices.size() && containingTagEndIndices[i] == this->offset) {
        this->closeTags.push_back(containingTags[i]);
      }
    }
  } else {
    this->offset = this->treeCursor.startIndex();
  }

  return true;
}

bool TreeSitterLanguageMode::LayerHighlightIterator::moveToSuccessor() {
  this->closeTags.clear();
  this->openTags.clear();

  while (!this->closeTags.size() && !this->openTags.size()) {
    if (this->atEnd) {
      if (this->moveRight_()) {
        const auto scopeId = this->currentScopeId_();
        if (scopeId) this->openTags.push_back(*scopeId);
        this->atEnd = false;
        this->moveDown_();
      } else if (this->moveUp_(true)) {
        this->atEnd = true;
      } else {
        return false;
      }
    } else if (!this->moveDown_()) {
      const auto scopeId = this->currentScopeId_();
      if (scopeId) this->closeTags.push_back(*scopeId);
      this->atEnd = true;
      this->moveUp_(false);
    }
  }

  if (this->atEnd) {
    this->offset = this->treeCursor.endIndex();
  } else {
    this->offset = this->treeCursor.startIndex();
  }

  return true;
}

Point TreeSitterLanguageMode::LayerHighlightIterator::getPosition() {
  if (this->atEnd) {
    return this->treeCursor.endPosition();
  } else {
    return this->treeCursor.startPosition();
  }
}

double TreeSitterLanguageMode::LayerHighlightIterator::compare(const NullLayerHighlightIterator *other_) {
  const LayerHighlightIterator *other = static_cast<const LayerHighlightIterator *>(other_);
  const double result = this->offset - other->offset;
  if (result != 0) return result;
  if (this->atEnd && !other->atEnd) return -1;
  if (other->atEnd && !this->atEnd) return 1;
  return this->languageLayer->depth - other->languageLayer->depth;
}

std::vector<int32_t> TreeSitterLanguageMode::LayerHighlightIterator::getCloseScopeIds() {
  return closeTags;
}

std::vector<int32_t> TreeSitterLanguageMode::LayerHighlightIterator::getOpenScopeIds() {
  return openTags;
}

bool TreeSitterLanguageMode::LayerHighlightIterator::moveUp_(bool atLastChild) {
  bool result = false;
  const double endIndex = this->treeCursor.endIndex();
  size_t depth = this->containingNodeEndIndices.size();

  // The iterator should not move up until it has visited all of the children of this node.
  while (
    depth > 1 &&
    (atLastChild || this->containingNodeEndIndices[depth - 2] == endIndex)
  ) {
    atLastChild = false;
    result = true;
    this->treeCursor.gotoParent();
    this->containingNodeTypes.pop_back();
    this->containingNodeChildIndices.pop_back();
    this->containingNodeEndIndices.pop_back();
    --depth;
    const auto scopeId = this->currentScopeId_();
    if (scopeId) this->closeTags.push_back(*scopeId);
  }
  return result;
}

bool TreeSitterLanguageMode::LayerHighlightIterator::moveDown_() {
  bool result = false;
  const double startIndex = this->treeCursor.startIndex();

  // Once the iterator has found a scope boundary, it needs to stay at the same
  // position, so it should not move down if the first child node starts later than the
  // current node.
  while (this->treeCursor.gotoFirstChild()) {
    if (
      (this->closeTags.size() || this->openTags.size()) &&
      this->treeCursor.startIndex() > startIndex
    ) {
      this->treeCursor.gotoParent();
      break;
    }

    result = true;
    this->containingNodeTypes.push_back(this->treeCursor.nodeType());
    this->containingNodeChildIndices.push_back(0);
    this->containingNodeEndIndices.push_back(this->treeCursor.endIndex());

    const auto scopeId = this->currentScopeId_();
    if (scopeId) this->openTags.push_back(*scopeId);
  }

  return result;
}

bool TreeSitterLanguageMode::LayerHighlightIterator::moveRight_() {
  if (this->treeCursor.gotoNextSibling()) {
    const size_t depth = this->containingNodeTypes.size();
    this->containingNodeTypes[depth - 1] = this->treeCursor.nodeType();
    this->containingNodeChildIndices[depth - 1]++;
    this->containingNodeEndIndices[depth - 1] = this->treeCursor.endIndex();
    return true;
  }
  return false;
}

optional<int32_t> TreeSitterLanguageMode::LayerHighlightIterator::currentScopeId_() {
  const auto value = this->languageLayer->grammar->scopeMap->get(
    this->containingNodeTypes,
    this->containingNodeChildIndices,
    this->treeCursor.nodeIsNamed()
  );
  const auto scopeName = /* applyLeafRules(std::move(value), this->treeCursor) */ std::move(value);
  const TSNode node = this->treeCursor.currentNode();
  if (!ts_node_child_count(node)) {
    return this->languageLayer->languageMode->grammar->idForScope(scopeName);
  } else if (scopeName) {
    return this->languageLayer->languageMode->grammar->idForScope(scopeName);
  }
  return optional<int32_t>();
}

/*
NullLayerHighlightIterator
*/

TreeSitterLanguageMode::NullLayerHighlightIterator::NullLayerHighlightIterator() {}

TreeSitterLanguageMode::NullLayerHighlightIterator::~NullLayerHighlightIterator() {}

bool TreeSitterLanguageMode::NullLayerHighlightIterator::seek(double, std::vector<int32_t> &, std::vector<double> &) {
  return false;
}

double TreeSitterLanguageMode::NullLayerHighlightIterator::compare(const NullLayerHighlightIterator *) {
  return 1;
}

bool TreeSitterLanguageMode::NullLayerHighlightIterator::moveToSuccessor() {
  return false;
}

Point TreeSitterLanguageMode::NullLayerHighlightIterator::getPosition() {
  return Point::INFINITY_;
}

std::vector<int32_t> TreeSitterLanguageMode::NullLayerHighlightIterator::getOpenScopeIds() {
  return {};
}

std::vector<int32_t> TreeSitterLanguageMode::NullLayerHighlightIterator::getCloseScopeIds() {
  return {};
}

static void insertContainingTag(int32_t tag, double index, std::vector<int32_t> &tags, std::vector<double> &indices) {
  const auto i = std::find_if(indices.begin(), indices.end(), [&](double existingIndex) { return existingIndex > index; });
  if (i == indices.end()) {
    tags.push_back(tag);
    indices.push_back(index);
  } else {
    tags.insert(tags.begin() + (i - indices.begin()), tag);
    indices.insert(i, index);
  }
}

static TreeSitterLanguageMode::NullLayerHighlightIterator *last(const std::vector<std::unique_ptr<TreeSitterLanguageMode::NullLayerHighlightIterator>> &array) {
  if (array.size() > 0) {
    return array[array.size() - 1].get();
  } else {
    return nullptr;
  }
}