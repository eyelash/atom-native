#include "tree-sitter-language-mode.h"
#include "tree-sitter-grammar.h"
#include "grammar-registry.h"
#include <tree-sitter.h>
#include <parser.h>
#include <text-buffer.h>

static void insertContainingTag(int32_t, double, std::vector<int32_t> &, std::vector<double> &);
template <typename T> static Range rangeForNode(T);
static bool nodeContainsIndices(TSNode, double, double);
static bool nodeIsSmaller(TSNode, TSNode);
template <typename T> static T *last(const std::vector<std::unique_ptr<T>> &);
template <typename T> static const T &last(const std::vector<T> &);

static const Range MAX_RANGE = Range(Point(0, 0), Point(INFINITY, INFINITY));

TreeSitterLanguageMode::TreeEdit::operator TSInputEdit() const {
  TSInputEdit edit;
  edit.start_point.row = startPosition.row;
  edit.start_point.column = startPosition.column * 2;
  edit.old_end_point.row = oldEndPosition.row;
  edit.old_end_point.column = oldEndPosition.column * 2;
  edit.new_end_point.row = newEndPosition.row;
  edit.new_end_point.column = newEndPosition.column * 2;
  edit.start_byte = startIndex * 2;
  edit.old_end_byte = oldEndIndex * 2;
  edit.new_end_byte = newEndIndex * 2;
  return edit;
}

TreeSitterLanguageMode::TreeSitterLanguageMode(TextBuffer *buffer, TreeSitterGrammar *grammar, GrammarRegistry *grammars) {
  this->buffer = buffer;
  this->grammar = grammar;
  this->grammarRegistry = grammars;
  this->rootLanguageLayer = new LanguageLayer(nullptr, this, grammar, 0);
  this->injectionsMarkerLayer = buffer->addMarkerLayer();
  this->rootLanguageLayer->update(nullptr);
}

TreeSitterLanguageMode::~TreeSitterLanguageMode() {
  //this->injectionsMarkerLayer->destroy();
  delete this->rootLanguageLayer;
  for (auto &entry : this->languageLayersByMarker) {
    delete entry.second;
  }
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
  for (Marker *marker : this->injectionsMarkerLayer->getMarkers()) {
    this->languageLayersByMarker[marker]->handleTextChange(edit, oldText, newText);
  }
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
  this->rootLanguageLayer->update(nullptr);
}

Tree TreeSitterLanguageMode::parse(const TSLanguage *language, const Tree &oldTree, const std::vector<TSRange> &ranges) {
  Parser *parser = /* PARSER_POOL.pop() || */ new Parser();
  parser->setLanguage(language);
  const Tree result = parser->parseTextBufferSync(this->buffer->buffer, oldTree,
    //syncTimeoutMicros: this.syncTimeoutMicros,
    ranges
  );

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

void TreeSitterLanguageMode::onDidChangeHighlighting(std::function<void(Range)> callback) {
  return this->didChangeHighlightingEmitter.on(callback);
}

std::string TreeSitterLanguageMode::classNameForScopeId(int32_t scopeId) {
  return this->grammar->classNameForScopeId(scopeId);
}

/*
Section - Commenting
*/

bool TreeSitterLanguageMode::isRowCommented(double row) {
  const auto range = this->firstNonWhitespaceRange(row);
  if (range) {
    TSNode firstNode = this->getSyntaxNodeContainingRange(*range);
    if (!ts_node_is_null(firstNode)) return std::string(ts_node_type(firstNode)).find("comment") != std::string::npos;
  }
  return false;
}

/*
Section - Indentation
*/

double TreeSitterLanguageMode::suggestedIndentForLineAtBufferRow(double row, const std::u16string &line, double tabLength) {
  return this->suggestedIndentForLineWithScopeAtBufferRow_(
    row,
    line,
    tabLength
  );
}

double TreeSitterLanguageMode::suggestedIndentForBufferRow(double row, double tabLength, bool skipBlankLines) {
  return this->suggestedIndentForLineWithScopeAtBufferRow_(
    row,
    this->buffer->lineForRow(row),
    tabLength,
    skipBlankLines
  );
}

optional<double> TreeSitterLanguageMode::suggestedIndentForEditedBufferRow(double bufferRow, double tabLength) {
  const std::u16string line = this->buffer->lineForRow(bufferRow);
  const double currentIndentLevel = this->indentLevelForLine(line, tabLength);
  if (currentIndentLevel == 0) return optional<double>();

  const Regex &decreaseIndentRegex = this->grammar->decreaseIndentRegex;
  if (!decreaseIndentRegex) return optional<double>();

  if (!decreaseIndentRegex.match(line)) return optional<double>();

  const optional<double> precedingRow = this->buffer->previousNonBlankRow(bufferRow);
  if (!precedingRow) return optional<double>();

  const std::u16string precedingLine = this->buffer->lineForRow(*precedingRow);
  double desiredIndentLevel = this->indentLevelForLine(precedingLine, tabLength);

  const Regex &increaseIndentRegex = this->grammar->increaseIndentRegex;
  if (increaseIndentRegex) {
    if (!increaseIndentRegex.match(precedingLine)) desiredIndentLevel -= 1;
  }

  const Regex &decreaseNextIndentRegex = this->grammar->decreaseNextIndentRegex;
  if (decreaseNextIndentRegex) {
    if (decreaseNextIndentRegex.match(precedingLine))
      desiredIndentLevel -= 1;
  }

  if (desiredIndentLevel < 0) return 0;
  if (desiredIndentLevel >= currentIndentLevel) return optional<double>();
  return desiredIndentLevel;
}

double TreeSitterLanguageMode::suggestedIndentForLineWithScopeAtBufferRow_(
  double bufferRow,
  const std::u16string &line,
  double tabLength,
  bool skipBlankLines
) {
  const Regex &increaseIndentRegex = this->grammar->increaseIndentRegex;
  const Regex &decreaseIndentRegex = this->grammar->decreaseIndentRegex;
  const Regex &decreaseNextIndentRegex = this->grammar->decreaseNextIndentRegex;

  optional<double> precedingRow;
  if (skipBlankLines) {
    precedingRow = this->buffer->previousNonBlankRow(bufferRow);
    if (!precedingRow) return 0;
  } else {
    precedingRow = bufferRow - 1;
    if (*precedingRow < 0) return 0;
  }

  const std::u16string precedingLine = this->buffer->lineForRow(*precedingRow);
  double desiredIndentLevel = this->indentLevelForLine(precedingLine, tabLength);
  if (!increaseIndentRegex) return desiredIndentLevel;

  if (!this->isRowCommented(*precedingRow)) {
    if (increaseIndentRegex && increaseIndentRegex.match(precedingLine))
      desiredIndentLevel += 1;
    if (
      decreaseNextIndentRegex &&
      decreaseNextIndentRegex.match(precedingLine)
    )
      desiredIndentLevel -= 1;
  }

  if (!this->buffer->isRowBlank(*precedingRow)) {
    if (decreaseIndentRegex && decreaseIndentRegex.match(line))
      desiredIndentLevel -= 1;
  }

  return std::max(desiredIndentLevel, 0.0);
}

double TreeSitterLanguageMode::indentLevelForLine(const std::u16string &line, double tabLength) {
  double indentLength = 0;
  for (size_t i = 0, length = line.size(); i < length; i++) {
    const char16_t char_ = line[i];
    if (char_ == u'\t') {
      indentLength += tabLength - std::fmod(indentLength, tabLength);
    } else if (char_ == u' ') {
      indentLength++;
    } else {
      break;
    }
  }
  return indentLength / tabLength;
}

/*
Section - Folding
*/

void TreeSitterLanguageMode::forEachTreeWithRange_(Range range, std::function<void(const Tree &, TreeSitterGrammar *)> callback) {
  if (this->rootLanguageLayer->tree) {
    callback(this->rootLanguageLayer->tree, this->rootLanguageLayer->grammar);
  }

  const auto injectionMarkers = this->injectionsMarkerLayer->findMarkers({
    intersectsRange(range)
  });

  for (Marker *injectionMarker : injectionMarkers) {
    LanguageLayer *languageLayer = this->languageLayersByMarker[injectionMarker];
    const Tree &tree = languageLayer->tree;
    TreeSitterGrammar *grammar = languageLayer->grammar;
    if (tree) callback(tree, grammar);
  }
}

/*
Section - Syntax Tree APIs
*/

TSNode TreeSitterLanguageMode::getSyntaxNodeContainingRange(Range range, std::function<bool(TSNode, TreeSitterGrammar *)> where) {
  return this->getSyntaxNodeAndGrammarContainingRange(range, where).first;
}

std::pair<TSNode, TreeSitterGrammar *> TreeSitterLanguageMode::getSyntaxNodeAndGrammarContainingRange(Range range, std::function<bool(TSNode, TreeSitterGrammar *)> where) {
  const double startIndex = this->buffer->characterIndexForPosition(range.start);
  const double endIndex = this->buffer->characterIndexForPosition(range.end);
  const double searchEndIndex = std::max(0.0, endIndex - 1);

  TSNode smallestNode = {};
  TreeSitterGrammar *smallestNodeGrammar = this->grammar;
  this->forEachTreeWithRange_(range, [&](const Tree &tree, TreeSitterGrammar *grammar) {
    TSNode node = ts_node_descendant_for_byte_range(tree.rootNode(), startIndex * 2, searchEndIndex * 2);
    while (!ts_node_is_null(node)) {
      if (
        nodeContainsIndices(node, startIndex, endIndex) &&
        where(node, grammar)
      ) {
        if (nodeIsSmaller(node, smallestNode)) {
          smallestNode = node;
          smallestNodeGrammar = grammar;
        }
        break;
      }
      node = ts_node_parent(node);
    }
  });

  return { smallestNode, smallestNodeGrammar };
}

optional<Range> TreeSitterLanguageMode::getRangeForSyntaxNodeContainingRange(Range range) {
  TSNode node = this->getSyntaxNodeContainingRange(range);
  return !ts_node_is_null(node) ? rangeForNode(node) : optional<Range>();
}

TSNode TreeSitterLanguageMode::getSyntaxNodeAtPosition(Point position, std::function<bool(TSNode, TreeSitterGrammar *)> where) {
  return this->getSyntaxNodeContainingRange(
    Range(position, position),
    where
  );
}

Grammar *TreeSitterLanguageMode::getGrammar() {
  return this->grammar;
}

/*
Section - Private
*/

optional<NativeRange> TreeSitterLanguageMode::firstNonWhitespaceRange(double row) {
  return this->buffer->findInRangeSync(
    Regex(u"\\S"),
    Range(Point(row, 0), Point(row, INFINITY))
  );
}

TreeSitterGrammar *TreeSitterLanguageMode::grammarForLanguageString(const std::u16string &languageString) {
  return this->grammarRegistry->treeSitterGrammarForLanguageString(
    languageString
  );
}

void TreeSitterLanguageMode::emitRangeUpdate(Range range) {
  const double startRow = range.start.row;
  const double endRow = range.end.row;
  for (double row = startRow; row < endRow; row++) {
    //this.isFoldableCache[row] = undefined;
  }
  this->didChangeHighlightingEmitter.emit(range);
}

/*
LanguageLayer
*/

TreeSitterLanguageMode::LanguageLayer::LanguageLayer(Marker *marker, TreeSitterLanguageMode *languageMode, TreeSitterGrammar *grammar, double depth) {
  this->marker = marker;
  this->languageMode = languageMode;
  this->grammar = grammar;
  this->depth = depth;
}

TreeSitterLanguageMode::LanguageLayer::~LanguageLayer() {}

std::unique_ptr<TreeSitterLanguageMode::LayerHighlightIterator> TreeSitterLanguageMode::LanguageLayer::buildHighlightIterator() {
  return std::unique_ptr<TreeSitterLanguageMode::LayerHighlightIterator>(new LayerHighlightIterator(this, this->tree.walk()));
}

void TreeSitterLanguageMode::LanguageLayer::handleTextChange(const TreeEdit &edit, const std::u16string &oldText, const std::u16string &newText) {
  const Point startPosition = edit.startPosition, oldEndPosition = edit.oldEndPosition, newEndPosition = edit.newEndPosition;

  if (this->tree) {
    this->tree.edit(edit);
    if (this->editedRange) {
      if (startPosition.isLessThan(this->editedRange->start)) {
        this->editedRange->start = startPosition;
      }
      if (oldEndPosition.isLessThan(this->editedRange->end)) {
        this->editedRange->end = newEndPosition.traverse(
          this->editedRange->end.traversalFrom(oldEndPosition)
        );
      } else {
        this->editedRange->end = newEndPosition;
      }
    } else {
      this->editedRange = Range(startPosition, newEndPosition);
    }
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

void TreeSitterLanguageMode::LanguageLayer::destroy() {
  this->tree = Tree();
  this->marker->destroy();
  for (Marker *marker : this->languageMode->injectionsMarkerLayer->getMarkers()) {
    if (this->languageMode->parentLanguageLayersByMarker[marker] == this) {
      this->languageMode->languageLayersByMarker[marker]->destroy();
    }
  }
}

void TreeSitterLanguageMode::LanguageLayer::update(NodeRangeSet *nodeRangeSet) {
  this->performUpdate_(nodeRangeSet);
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

void TreeSitterLanguageMode::LanguageLayer::performUpdate_(NodeRangeSet *nodeRangeSet /* , params */) {
  std::vector<TSRange> includedRanges;
  if (nodeRangeSet) {
    includedRanges = nodeRangeSet->getRanges(this->languageMode->buffer);
    if (includedRanges.size() == 0) {
      const Range range = this->marker->getRange();
      this->destroy();
      this->languageMode->emitRangeUpdate(range);
      return;
    }
  }

  auto affectedRange = this->editedRange;
  this->editedRange = optional<Range>();

  //this.patchSinceCurrentParseStarted = new Patch();
  auto tree = this->languageMode->parse(
    this->grammar->languageModule,
    this->tree,
    includedRanges
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
    const auto rangesWithSyntaxChanges = this->tree.getChangedRanges(tree);
    this->tree = tree;

    if (rangesWithSyntaxChanges.size() > 0) {
      for (const auto range : rangesWithSyntaxChanges) {
        this->languageMode->emitRangeUpdate(rangeForNode(range));
      }

      const Range combinedRangeWithSyntaxChange = Range(
        startPosition(rangesWithSyntaxChanges[0]),
        endPosition(last(rangesWithSyntaxChanges))
      );

      if (affectedRange) {
        this->languageMode->emitRangeUpdate(*affectedRange);
        affectedRange = affectedRange->union_(combinedRangeWithSyntaxChange);
      } else {
        affectedRange = combinedRangeWithSyntaxChange;
      }
    }
  } else {
    this->tree = tree;
    this->languageMode->emitRangeUpdate(rangeForNode(tree.rootNode()));
    if (includedRanges.size()) {
      affectedRange = Range(
        startPosition(includedRanges[0]),
        endPosition(last(includedRanges))
      );
    } else {
      affectedRange = MAX_RANGE;
    }
  }

  if (affectedRange) {
    /* const injectionPromise = */ this->populateInjections_(
      *affectedRange,
      nodeRangeSet
    );
    /*if (injectionPromise) {
      params.async = true;
      return injectionPromise;
    }*/
  }
}

void TreeSitterLanguageMode::LanguageLayer::populateInjections_(Range range, NodeRangeSet *nodeRangeSet) {
  auto existingInjectionMarkers = this->languageMode->injectionsMarkerLayer
    ->findMarkers({ intersectsRange(range) });
  existingInjectionMarkers.erase(std::remove_if(existingInjectionMarkers.begin(), existingInjectionMarkers.end(), [this](Marker *marker) {
    return this->languageMode->parentLanguageLayersByMarker[marker] != this;
  }), existingInjectionMarkers.end());

  if (existingInjectionMarkers.size() > 0) {
    range = range.union_(
      Range(
        existingInjectionMarkers[0]->getRange().start,
        last(existingInjectionMarkers)->getRange().end
      )
    );
  }

  std::unordered_map<Marker *, NodeRangeSet *> markersToUpdate;
  const auto nodes = descendantsOfType(this->tree.rootNode(),
    keys(this->grammar->injectionPointsByType),
    range.start,
    range.end
  );

  double existingInjectionMarkerIndex = 0;
  for (TSNode node : nodes) {
    for (const auto &injectionPoint : this->grammar->injectionPointsByType[
      ts_node_type(node)
    ]) {
      const std::u16string languageName = injectionPoint.language(node);
      //if (!languageName) continue;

      TreeSitterGrammar *grammar = this->languageMode->grammarForLanguageString(
        languageName
      );
      if (!grammar) continue;

      const auto injectionNodes = injectionPoint.content(node);
      if (!injectionNodes.size()) continue;

      const Range injectionRange = rangeForNode(node);

      Marker *marker = nullptr;
      for (
        double i = existingInjectionMarkerIndex,
          n = existingInjectionMarkers.size();
        i < n;
        i++
      ) {
        Marker *existingMarker = existingInjectionMarkers[i];
        const int comparison = existingMarker->getRange().compare(injectionRange);
        if (comparison > 0) {
          break;
        } else if (comparison == 0) {
          existingInjectionMarkerIndex = i;
          if (this->languageMode->languageLayersByMarker[existingMarker]->grammar == grammar) {
            marker = existingMarker;
            break;
          }
        } else {
          existingInjectionMarkerIndex = i;
        }
      }

      if (!marker) {
        marker = this->languageMode->injectionsMarkerLayer->markRange(
          injectionRange
        );
        this->languageMode->languageLayersByMarker[marker] = new LanguageLayer(
          marker,
          this->languageMode,
          grammar,
          this->depth + 1
        );
        this->languageMode->parentLanguageLayersByMarker[marker] = this;
      }

      markersToUpdate[marker] =
        new NodeRangeSet(
          nodeRangeSet,
          injectionNodes,
          /* injectionPoint.newlinesBetween */ false,
          /* injectionPoint.includeChildren */ false
        );
    }
  }

  for (Marker *marker : existingInjectionMarkers) {
    if (!markersToUpdate.count(marker)) {
      this->languageMode->emitRangeUpdate(marker->getRange());
      this->languageMode->languageLayersByMarker[marker]->destroy();
    }
  }

  if (markersToUpdate.size() > 0) {
    //const promises = [];
    for (const auto &entry : markersToUpdate) {
      Marker *marker = entry.first;
      NodeRangeSet *nodeRangeSet = entry.second;
      this->languageMode->languageLayersByMarker[marker]->update(nodeRangeSet);
      delete nodeRangeSet;
    }
    //return Promise.all(promises);
  }
}

TreeSitterLanguageMode::TreeEdit TreeSitterLanguageMode::LanguageLayer::treeEditForBufferChange_(Point start, Point oldEnd, Point newEnd, const std::u16string &oldText, const std::u16string &newText) {
  const double startIndex = this->languageMode->buffer->characterIndexForPosition(
    start
  );
  return {
    startIndex,
    startIndex + oldText.size(),
    startIndex + newText.size(),
    start,
    oldEnd,
    newEnd
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
  const auto injectionMarkers = this->languageMode->injectionsMarkerLayer->findMarkers(
    {
      intersectsRange(Range(targetPosition, Point(endRow + 1, 0)))
    }
  );

  std::vector<int32_t> containingTags;
  std::vector<double> containingTagStartIndices;
  const double targetIndex = this->languageMode->buffer->characterIndexForPosition(
    targetPosition
  );

  this->iterators.clear();
  std::unique_ptr<LayerHighlightIterator> iterator = this->languageMode->rootLanguageLayer->buildHighlightIterator();
  if (iterator->seek(targetIndex, containingTags, containingTagStartIndices)) {
    this->iterators.push_back(std::move(iterator));
  }

  // Populate the iterators array with all of the iterators whose syntax
  // trees span the given position.
  for (Marker *marker : injectionMarkers) {
    auto iterator = this->languageMode->languageLayersByMarker[marker]->buildHighlightIterator();
    if (
      iterator->seek(targetIndex, containingTags, containingTagStartIndices)
    ) {
      this->iterators.push_back(std::move(iterator));
    }
  }

  // Sort the iterators so that the last one in the array is the earliest
  // in the document, and represents the current position.
  std::sort(this->iterators.begin(), this->iterators.end(), [](const std::unique_ptr<LayerHighlightIterator> &a, const std::unique_ptr<LayerHighlightIterator> &b) { return b->compare(a.get()) < 0; });
  this->detectCoveredScope();

  return containingTags;
}

void TreeSitterLanguageMode::HighlightIterator::moveToSuccessor() {
  // Advance the earliest layer iterator to its next scope boundary.
  LayerHighlightIterator *leader = last(this->iterators);

  // Maintain the sorting of the iterators by their position in the document.
  if (leader->moveToSuccessor()) {
    const size_t leaderIndex = this->iterators.size() - 1;
    size_t i = leaderIndex;
    while (i > 0 && this->iterators[i - 1]->compare(leader) < 0) i--;
    if (i < leaderIndex) {
      //this->iterators.splice(i, 0, this->iterators.pop());
      std::unique_ptr<LayerHighlightIterator> iterator = pop(this->iterators);
      this->iterators.insert(this->iterators.begin() + i, std::move(iterator));
    }
  } else {
    // If the layer iterator was at the end of its syntax tree, then remove
    // it from the array.
    this->iterators.pop_back();
  }

  this->detectCoveredScope();
}

void TreeSitterLanguageMode::HighlightIterator::detectCoveredScope() {
  const size_t layerCount = this->iterators.size();
  if (layerCount > 1) {
    LayerHighlightIterator *first = this->iterators[layerCount - 1].get();
    LayerHighlightIterator *next = this->iterators[layerCount - 2].get();
    if (
      next->offset == first->offset &&
      next->atEnd == first->atEnd &&
      next->depth > first->depth &&
      !next->isAtInjectionBoundary()
    ) {
      this->currentScopeIsCovered = true;
      return;
    }
  }
  this->currentScopeIsCovered = false;
}

Point TreeSitterLanguageMode::HighlightIterator::getPosition() {
  LayerHighlightIterator *iterator = last(this->iterators);
  if (iterator) {
    return iterator->getPosition();
  } else {
    return Point::INFINITY_;
  }
}

std::vector<int32_t> TreeSitterLanguageMode::HighlightIterator::getCloseScopeIds() {
  LayerHighlightIterator *iterator = last(this->iterators);
  if (iterator && !this->currentScopeIsCovered) {
    return iterator->getCloseScopeIds();
  }
  return {};
}

std::vector<int32_t> TreeSitterLanguageMode::HighlightIterator::getOpenScopeIds() {
  LayerHighlightIterator *iterator = last(this->iterators);
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

double TreeSitterLanguageMode::LayerHighlightIterator::compare(const LayerHighlightIterator *other) {
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

bool TreeSitterLanguageMode::LayerHighlightIterator::isAtInjectionBoundary() {
  return this->containingNodeTypes.size() == 1;
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
  std::shared_ptr<SyntaxScopeMap::Result> value = this->languageLayer->grammar->scopeMap->get(
    this->containingNodeTypes,
    this->containingNodeChildIndices,
    this->treeCursor.nodeIsNamed()
  );
  TextBuffer *buffer = this->languageLayer->languageMode->buffer;

  const auto scopeName = value ? value->applyLeafRules(buffer, this->treeCursor) : optional<std::string>();
  const TSNode node = this->treeCursor.currentNode();
  if (!ts_node_child_count(node)) {
    return this->languageLayer->languageMode->grammar->idForScope(scopeName);
  } else if (scopeName) {
    return this->languageLayer->languageMode->grammar->idForScope(scopeName);
  }
  return optional<int32_t>();
}

static TSRange RangeFromJS(double startIndex, double endIndex, const Point &startPosition, const Point &endPosition) {
  TSRange result;
  result.start_point.row = startPosition.row;
  result.start_point.column = startPosition.column * 2;
  result.end_point.row = endPosition.row;
  result.end_point.column = endPosition.column * 2;
  result.start_byte = startIndex * 2;
  result.end_byte = endIndex * 2;
  return result;
}

TreeSitterLanguageMode::NodeRangeSet::NodeRangeSet(NodeRangeSet *previous, const std::vector<TSNode> &nodes, bool newlinesBetween, bool includeChildren) {
  this->previous = previous;
  this->nodes = nodes;
  this->newlinesBetween = newlinesBetween;
  this->includeChildren = includeChildren;
}

std::vector<TSRange> TreeSitterLanguageMode::NodeRangeSet::getRanges(TextBuffer *buffer) {
  const std::vector<TSRange> previousRanges = this->previous ? this->previous->getRanges(buffer) : std::vector<TSRange>();
  std::vector<TSRange> result;

  for (TSNode node : this->nodes) {
    Point position = startPosition(node);
    double index = startIndex(node);

    if (!this->includeChildren) {
      for (TSNode child : children(node)) {
        const double nextIndex = startIndex(child);
        if (nextIndex > index) {
          this->pushRange_(buffer, previousRanges, result, RangeFromJS(
            index,
            nextIndex,
            position,
            startPosition(child)
          ));
        }
        position = endPosition(child);
        index = endIndex(child);
      }
    }

    if (endIndex(node) > index) {
      this->pushRange_(buffer, previousRanges, result, RangeFromJS(
        index,
        endIndex(node),
        position,
        endPosition(node)
      ));
    }
  }

  return result;
}

void TreeSitterLanguageMode::NodeRangeSet::pushRange_(TextBuffer *buffer, const std::vector<TSRange> &previousRanges, std::vector<TSRange> &newRanges, TSRange newRange) {
  if (previousRanges.empty()) {
    if (this->newlinesBetween) {
      this->ensureNewline_(buffer, newRanges, startIndex(newRange), startPosition(newRange));
    }
    newRanges.push_back(newRange);
    return;
  }

  for (TSRange previousRange : previousRanges) {
    if (endIndex(previousRange) <= startIndex(newRange)) continue;
    if (startIndex(previousRange) >= endIndex(newRange)) break;
    const double startIndex = std::max(
      ::startIndex(previousRange),
      ::startIndex(newRange)
    );
    const double endIndex = std::min(::endIndex(previousRange), ::endIndex(newRange));
    const Point startPosition = Point::max(
      ::startPosition(previousRange),
      ::startPosition(newRange)
    );
    const Point endPosition = Point::min(
      ::endPosition(previousRange),
      ::endPosition(newRange)
    );
    if (this->newlinesBetween) {
      this->ensureNewline_(buffer, newRanges, startIndex, startPosition);
    }
    newRanges.push_back(RangeFromJS(startIndex, endIndex, startPosition, endPosition));
  }
}

// For injection points with `newlinesBetween` enabled, ensure that a
// newline is included between each disjoint range.
void TreeSitterLanguageMode::NodeRangeSet::ensureNewline_(TextBuffer *buffer, std::vector<TSRange> &newRanges, double startIndex, Point startPosition) {
  /*const lastRange = newRanges[newRanges.length - 1];
  if (lastRange && lastRange.endPosition.row < startPosition.row) {
    newRanges.push({
      startPosition: new Point(
        startPosition.row - 1,
        buffer.lineLengthForRow(startPosition.row - 1)
      ),
      endPosition: new Point(startPosition.row, 0),
      startIndex: startIndex - startPosition.column - 1,
      endIndex: startIndex - startPosition.column
    });
  }*/
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

template <typename T> static Range rangeForNode(T node) {
  return Range(startPosition(node), endPosition(node));
}

static bool nodeContainsIndices(TSNode node, double start, double end) {
  if (startIndex(node) < start) return endIndex(node) >= end;
  if (startIndex(node) == start) return endIndex(node) > end;
  return false;
}

static bool nodeIsSmaller(TSNode left, TSNode right) {
  if (ts_node_is_null(left)) return false;
  if (ts_node_is_null(right)) return true;
  return endIndex(left) - startIndex(left) < endIndex(right) - startIndex(right);
}

template <typename T> static T *last(const std::vector<std::unique_ptr<T>> &array) {
  if (array.size() > 0) {
    return array[array.size() - 1].get();
  } else {
    return nullptr;
  }
}
template <typename T> static const T &last(const std::vector<T> &array) {
  return array[array.size() - 1];
}
