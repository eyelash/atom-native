#include "display-layer.h"
#include "text-buffer.h"
#include "screen-line-builder.h"
#include "marker-layer.h"
#include "display-marker-layer.h"
#include "marker.h"
#include "point-helpers.h"
#include "is-character-pair.h"
#include "helpers.h"
#include "constants.h"
#include "language-mode.h"

static bool isWordStart(char16_t, char16_t);
static double unitRatio(char16_t);

DisplayLayer::DisplayLayer(unsigned id, TextBuffer *buffer) {
  this->id = id;
  this->buffer = buffer;
  this->screenLineBuilder = new ScreenLineBuilder(this);
  this->nextBuiltInScopeId = 1;
  this->tabLength = 4;
  this->softWrapColumn = INFINITY;
  this->softWrapHangingIndent = 0;
  this->showIndentGuides = false;
  this->ratioForCharacter = unitRatio;
  this->isWrapBoundary = isWordStart;
  this->foldCharacter = u'\u22EF';
  this->eolInvisibles[u"\r"] = this->invisibles.cr;
  this->eolInvisibles[u"\n"] = this->invisibles.eol;
  //this->eolInvisibles[u"\r\n"] = this->invisibles.cr + this->invisibles.eol;
  this->foldsMarkerLayer = buffer->addMarkerLayer(/*{
    maintainHistory: false,
    persistent: true,
    destroyInvalidatedMarkers: true
  }*/);
  this->spatialIndex = new Patch(/*{mergeAdjacentHunks: false}*/);
  this->rightmostScreenPosition = Point(0, 0);
  this->indexedBufferRowCount = 0;
}

DisplayLayer::~DisplayLayer() {
  delete this->screenLineBuilder;
  for (auto &displayMarkerLayer : this->displayMarkerLayersById) {
    delete displayMarkerLayer.second;
  }
  delete this->spatialIndex;
}

void DisplayLayer::clearSpatialIndex() {
  this->indexedBufferRowCount = 0;
  this->spatialIndex->splice_old(Point::ZERO, Point::INFINITY_, Point::INFINITY_);
  this->cachedScreenLines.resize(0);
  this->screenLineLengths.resize(0);
  this->tabCounts.resize(0);
  this->rightmostScreenPosition = Point(0, 0);
}

void DisplayLayer::bufferDidChangeLanguageMode(LanguageMode *) {
  this->cachedScreenLines.resize(0);
  //if (this.languageModeDisposable) this.languageModeDisposable.dispose()
  this->buffer->languageMode->onDidChangeHighlighting([this](Range bufferRange) {
    this->populateSpatialIndexIfNeeded(bufferRange.end.row + 1, INFINITY);
    const double startBufferRow = this->findBoundaryPrecedingBufferRow(bufferRange.start.row);
    const double endBufferRow = this->findBoundaryFollowingBufferRow(bufferRange.end.row + 1);
    const double startRow = this->translateBufferPositionWithSpatialIndex(Point(startBufferRow, 0), ClipDirection::backward).row;
    const double endRow = this->translateBufferPositionWithSpatialIndex(Point(endBufferRow, 0), ClipDirection::backward).row;
    const Point extent = Point(endRow - startRow, 0);
    spliceArray(this->cachedScreenLines, startRow, extent.row, std::vector<optional<ScreenLine>>(extent.row));
    this->didChange({
      Point(startRow, 0),
      extent,
      extent
    });
  });
}

DisplayMarkerLayer *DisplayLayer::addMarkerLayer() {
  DisplayMarkerLayer *markerLayer = new DisplayMarkerLayer(this, this->buffer->addMarkerLayer(), true);
  this->displayMarkerLayersById[markerLayer->id] = markerLayer;
  return markerLayer;
}

DisplayMarkerLayer *DisplayLayer::getMarkerLayer(unsigned id) {
  auto iter = this->displayMarkerLayersById.find(id);
  if (iter != this->displayMarkerLayersById.end()) {
    return iter->second;
  } else {
    MarkerLayer *bufferMarkerLayer = this->buffer->getMarkerLayer(id);
    if (bufferMarkerLayer) {
      DisplayMarkerLayer *displayMarkerLayer = new DisplayMarkerLayer(this, bufferMarkerLayer, false);
      this->displayMarkerLayersById[id] = displayMarkerLayer;
      return displayMarkerLayer;
    }
  }
  return nullptr;
}

Range DisplayLayer::bufferRangeForFold(unsigned foldId) {
  return this->foldsMarkerLayer->getMarkerRange(foldId);
}

Point DisplayLayer::translateBufferPosition(Point bufferPosition, ClipDirection clipDirection) {
  bufferPosition = this->buffer->clipPosition(bufferPosition);
  this->populateSpatialIndexIfNeeded(bufferPosition.row + 1, INFINITY);

  const double columnDelta = this->getClipColumnDelta(bufferPosition, clipDirection);
  if (columnDelta != 0) {
    bufferPosition = Point(bufferPosition.row, bufferPosition.column + columnDelta);
  }

  Point screenPosition = this->translateBufferPositionWithSpatialIndex(bufferPosition, clipDirection);
  const double tabCount = this->tabCounts[screenPosition.row];
  if (tabCount > 0) {
    screenPosition = this->expandHardTabs(screenPosition, bufferPosition, tabCount);
  }

  return screenPosition;
}

Point DisplayLayer::translateBufferPositionWithSpatialIndex(Point bufferPosition, ClipDirection clipDirection) {
  auto hunk = this->spatialIndex->grab_change_starting_before_old_position(bufferPosition);
  if (hunk) {
    if (compare(bufferPosition, hunk->old_end) < 0) {
      if (compare(hunk->old_start, bufferPosition) == 0) {
        return hunk->new_start;
      } else { // hunk is a fold
        if (clipDirection == ClipDirection::backward) {
          return hunk->new_start;
        } else if (clipDirection == ClipDirection::forward) {
          return hunk->new_end;
        } else {
          const Point distanceFromFoldStart = traversal(bufferPosition, hunk->old_start);
          const Point distanceToFoldEnd = traversal(hunk->old_end, bufferPosition);
          if (compare(distanceFromFoldStart, distanceToFoldEnd) <= 0) {
            return hunk->new_start;
          } else {
            return hunk->new_end;
          }
        }
      }
    } else {
      return traverse(hunk->new_end, traversal(bufferPosition, hunk->old_end));
    }
  } else {
    return bufferPosition;
  }
}

Range DisplayLayer::translateBufferRange(Range bufferRange, ClipDirection clipDirection) {
  return Range(
    this->translateBufferPosition(bufferRange.start, clipDirection),
    this->translateBufferPosition(bufferRange.end, clipDirection)
  );
}

Point DisplayLayer::translateScreenPosition(Point screenPosition, ClipDirection clipDirection, bool skipSoftWrapIndentation) {
  this->populateSpatialIndexIfNeeded(this->buffer->getLineCount(), screenPosition.row + 1);
  screenPosition = this->constrainScreenPosition(screenPosition, clipDirection);
  const double tabCount = this->tabCounts[screenPosition.row];
  if (tabCount > 0) {
    screenPosition = this->collapseHardTabs(screenPosition, tabCount, clipDirection);
  }
  const Point bufferPosition = this->translateScreenPositionWithSpatialIndex(screenPosition, clipDirection, skipSoftWrapIndentation);

  /*if (global.atom && bufferPosition.row >= this.buffer.getLineCount()) {
    global.atom.assert(false, 'Invalid translated buffer row', {
      bufferPosition, bufferLineCount: this.buffer.getLineCount()
    })
    return this.buffer.getEndPosition()
  }*/

  const double columnDelta = this->getClipColumnDelta(bufferPosition, clipDirection);
  if (columnDelta != 0) {
    return Point(bufferPosition.row, bufferPosition.column + columnDelta);
  } else {
    return bufferPosition;
  }
}

Point DisplayLayer::translateScreenPositionWithSpatialIndex(Point screenPosition, ClipDirection clipDirection, bool skipSoftWrapIndentation) {
  auto hunk = this->spatialIndex->grab_change_starting_before_new_position(screenPosition);
  if (hunk) {
    if (compare(screenPosition, hunk->new_end) < 0) {
      if (this->isSoftWrapHunk(*hunk)) {
        if ((clipDirection == ClipDirection::backward && !skipSoftWrapIndentation) ||
            (clipDirection == ClipDirection::closest && isEqual(hunk->new_start, screenPosition))) {
          return this->translateScreenPositionWithSpatialIndex(traverse(hunk->new_start, Point(0, -1)), clipDirection, skipSoftWrapIndentation);
        } else {
          return hunk->old_start;
        }
      } else { // Hunk is a fold. Since folds are 1 character on screen, we're at the start.
        return hunk->old_start;
      }
    } else {
      return traverse(hunk->old_end, traversal(screenPosition, hunk->new_end));
    }
  } else {
    return screenPosition;
  }
}

Range DisplayLayer::translateScreenRange(Range screenRange, ClipDirection clipDirection, bool skipSoftWrapIndentation) {
  return Range(
    this->translateScreenPosition(screenRange.start, clipDirection, skipSoftWrapIndentation),
    this->translateScreenPosition(screenRange.end, clipDirection, skipSoftWrapIndentation)
  );
}

Point DisplayLayer::clipScreenPosition(Point screenPosition) {
  return this->translateBufferPosition(
    this->translateScreenPosition(screenPosition)
  );
}

Point DisplayLayer::constrainScreenPosition(Point screenPosition, ClipDirection clipDirection) {
  double row = screenPosition.row, column = screenPosition.column;

  if (row < 0) {
    return Point(0, 0);
  }

  const double maxRow = this->screenLineLengths.size() - 1.0;
  if (row > maxRow) {
    return Point(maxRow, this->screenLineLengths[maxRow]);
  }

  if (column < 0) {
    return Point(row, 0);
  }

  const double maxColumn = this->screenLineLengths[row];
  if (column > maxColumn) {
    if (clipDirection == ClipDirection::forward && row < maxRow) {
      return Point(row + 1, 0);
    } else {
      return Point(row, maxColumn);
    }
  }

  return screenPosition;
}

Point DisplayLayer::expandHardTabs(Point targetScreenPosition, Point targetBufferPosition, double tabCount) {
  const Point screenRowStart = Point(targetScreenPosition.row, 0);
  auto hunks = this->spatialIndex->grab_changes_in_new_range(screenRowStart, targetScreenPosition);
  double hunkIndex = 0;
  double unexpandedScreenColumn = 0;
  double expandedScreenColumn = 0;
  const Point bufferPosition = this->translateScreenPositionWithSpatialIndex(screenRowStart);
  double bufferRow = bufferPosition.row, bufferColumn = bufferPosition.column;
  auto bufferLine = this->buffer->lineForRow(bufferRow);

  while (tabCount > 0) {
    if (unexpandedScreenColumn == targetScreenPosition.column) {
      break;
    }

    const Patch::Change *nextHunk = hunkIndex < hunks.size() ? &hunks[hunkIndex] : nullptr;
    if (nextHunk && nextHunk->old_start.row == bufferRow && nextHunk->old_start.column == bufferColumn) {
      if (this->isSoftWrapHunk(*nextHunk)) {
        //if (hunkIndex != 0) throw new Error('Unexpected soft wrap hunk');
        unexpandedScreenColumn = hunks[0].new_end.column;
        expandedScreenColumn = unexpandedScreenColumn;
      } else {
        bufferRow = nextHunk->old_end.row, bufferColumn = nextHunk->old_end.column;
        bufferLine = this->buffer->lineForRow(bufferRow);
        unexpandedScreenColumn++;
        expandedScreenColumn++;
      }

      hunkIndex++;
      continue;
    }

    if ((*bufferLine)[bufferColumn] == u'\t') {
      expandedScreenColumn += (this->tabLength - std::fmod(expandedScreenColumn, this->tabLength));
      tabCount--;
    } else {
      expandedScreenColumn++;
    }
    unexpandedScreenColumn++;
    bufferColumn++;
  }

  expandedScreenColumn += targetScreenPosition.column - unexpandedScreenColumn;
  if (expandedScreenColumn == targetScreenPosition.column) {
    return targetScreenPosition;
  } else {
    return Point(targetScreenPosition.row, expandedScreenColumn);
  }
}

Point DisplayLayer::collapseHardTabs(Point targetScreenPosition, double tabCount, ClipDirection clipDirection) {
  const Point screenRowStart = Point(targetScreenPosition.row, 0);
  const Point screenRowEnd = Point(targetScreenPosition.row, this->screenLineLengths[targetScreenPosition.row]);

  auto hunks = this->spatialIndex->grab_changes_in_new_range(screenRowStart, screenRowEnd);
  double hunkIndex = 0;
  double unexpandedScreenColumn = 0;
  double expandedScreenColumn = 0;
  const Point bufferPosition = this->translateScreenPositionWithSpatialIndex(screenRowStart);
  double bufferRow = bufferPosition.row, bufferColumn = bufferPosition.column;
  auto bufferLine = this->buffer->lineForRow(bufferRow);

  while (tabCount > 0) {
    if (expandedScreenColumn == targetScreenPosition.column) {
      break;
    }

    const Patch::Change *nextHunk = hunkIndex < hunks.size() ? &hunks[hunkIndex] : nullptr;
    if (nextHunk && nextHunk->old_start.row == bufferRow && nextHunk->old_start.column == bufferColumn) {
      if (this->isSoftWrapHunk(*nextHunk)) {
        //if (hunkIndex != 0) throw new Error('Unexpected soft wrap hunk')
        unexpandedScreenColumn = std::min(targetScreenPosition.column, (double)nextHunk->new_end.column);
        expandedScreenColumn = unexpandedScreenColumn;
      } else {
        bufferRow = nextHunk->old_end.row, bufferColumn = nextHunk->old_end.column;
        bufferLine = this->buffer->lineForRow(bufferRow);
        unexpandedScreenColumn++;
        expandedScreenColumn++;
      }
      hunkIndex++;
      continue;
    }

    if ((*bufferLine)[bufferColumn] == u'\t') {
      const double nextTabStopColumn = expandedScreenColumn + this->tabLength - std::fmod(expandedScreenColumn, this->tabLength);
      if (nextTabStopColumn > targetScreenPosition.column) {
        if (clipDirection == ClipDirection::backward) {
          return Point(targetScreenPosition.row, unexpandedScreenColumn);
        } else if (clipDirection == ClipDirection::forward) {
          return Point(targetScreenPosition.row, unexpandedScreenColumn + 1);
        } else {
          if (targetScreenPosition.column > std::ceil((nextTabStopColumn + expandedScreenColumn) / 2)) {
            return Point(targetScreenPosition.row, unexpandedScreenColumn + 1);
          } else {
            return Point(targetScreenPosition.row, unexpandedScreenColumn);
          }
        }
      }
      expandedScreenColumn = nextTabStopColumn;
      tabCount--;
    } else {
      expandedScreenColumn++;
    }
    unexpandedScreenColumn++;
    bufferColumn++;
  }

  unexpandedScreenColumn += targetScreenPosition.column - expandedScreenColumn;
  if (unexpandedScreenColumn == targetScreenPosition.column) {
    return targetScreenPosition;
  } else {
    return Point(targetScreenPosition.row, unexpandedScreenColumn);
  }
}

double DisplayLayer::getClipColumnDelta(Point bufferPosition, ClipDirection clipDirection) {
  double row = bufferPosition.row, column = bufferPosition.column;

  // Treat paired unicode characters as atomic...
  char16_t character = this->buffer->getCharacterAtPosition(bufferPosition);
  char16_t previousCharacter = this->buffer->getCharacterAtPosition(Point(row, column - 1));
  if (previousCharacter && character && isCharacterPair(previousCharacter, character)) {
    if (clipDirection == ClipDirection::closest || clipDirection == ClipDirection::backward) {
      return -1;
    } else {
      return 1;
    }
  }

  // Clip atomic soft tabs...

  //if (!this->atomicSoftTabs) return 0;

  if (column * this->ratioForCharacter(' ') > this->softWrapColumn) {
    return 0;
  }

  for (Point position = Point(row, column); position.column >= 0; position.column--) {
    if (this->buffer->getCharacterAtPosition(position) != u' ') return 0;
  }

  double previousTabStop = column - std::fmod(column, this->tabLength);
  if (column == previousTabStop) return 0;
  double nextTabStop = previousTabStop + this->tabLength;

  // If there is a non-whitespace character before the next tab stop,
  // don't this whitespace as a soft tab
  for (Point position = Point(row, column); position.column < nextTabStop; position.column++) {
    if (this->buffer->getCharacterAtPosition(position) != u' ') return 0;
  }

  double clippedColumn;
  if (clipDirection == ClipDirection::closest) {
    if (column - previousTabStop > this->tabLength / 2) {
      clippedColumn = nextTabStop;
    } else {
      clippedColumn = previousTabStop;
    }
  } else if (clipDirection == ClipDirection::backward) {
    clippedColumn = previousTabStop;
  } else if (clipDirection == ClipDirection::forward) {
    clippedColumn = nextTabStop;
  }

  return clippedColumn - column;
}

/*getText (startRow, endRow) {
  return this.getScreenLines(startRow, endRow).map((line) => line.lineText).join('\n')
}*/

double DisplayLayer::lineLengthForScreenRow(double screenRow) {
  this->populateSpatialIndexIfNeeded(this->buffer->getLineCount(), screenRow + 1);
  return this->screenLineLengths[screenRow];
}

double DisplayLayer::getLastScreenRow() {
  this->populateSpatialIndexIfNeeded(this->buffer->getLineCount(), INFINITY);
  return this->screenLineLengths.size() - 1.0;
}

double DisplayLayer::getScreenLineCount() {
  this->populateSpatialIndexIfNeeded(this->buffer->getLineCount(), INFINITY);
  return this->screenLineLengths.size();
}

double DisplayLayer::getApproximateScreenLineCount() {
  if (this->indexedBufferRowCount > 0) {
    return std::floor(this->buffer->getLineCount() * this->screenLineLengths.size() / this->indexedBufferRowCount);
  } else {
    return this->buffer->getLineCount();
  }
}

Point DisplayLayer::getRightmostScreenPosition() {
  this->populateSpatialIndexIfNeeded(this->buffer->getLineCount(), INFINITY);
  return this->rightmostScreenPosition;
}

Point DisplayLayer::getApproximateRightmostScreenPosition() {
  return this->rightmostScreenPosition;
}

DisplayLayer::ScreenLine DisplayLayer::getScreenLine(double screenRow) {
  optional<ScreenLine> cachedScreenLine = screenRow >= 0 && screenRow < this->cachedScreenLines.size() ? this->cachedScreenLines[screenRow] : optional<ScreenLine>();
  return cachedScreenLine ? *cachedScreenLine : this->getScreenLines(screenRow, screenRow + 1)[0];
}

std::vector<DisplayLayer::ScreenLine> DisplayLayer::getScreenLines(double screenStartRow, double screenEndRow) {
  return this->screenLineBuilder->buildScreenLines(screenStartRow, screenEndRow);
}

std::vector<DisplayLayer::ScreenLine> DisplayLayer::getScreenLines(double screenStartRow) {
  return this->getScreenLines(screenStartRow, this->getScreenLineCount());
}

std::vector<double> DisplayLayer::bufferRowsForScreenRows(double startRow, double endRow) {
  this->populateSpatialIndexIfNeeded(this->buffer->getLineCount(), endRow);

  const Point startPosition = Point(startRow, 0);
  std::vector<double> bufferRows;
  double lastScreenRow = startRow;
  double lastBufferRow = this->translateScreenPositionWithSpatialIndex(startPosition).row;
  auto hunks = this->spatialIndex->grab_changes_in_new_range(startPosition, Point(endRow, 0));
  for (unsigned i = 0; i < hunks.size(); i++) {
    const auto& hunk = hunks[i];
    while (lastScreenRow <= hunk.new_start.row) {
      bufferRows.push_back(lastBufferRow);
      lastScreenRow++;
      lastBufferRow++;
    }

    lastBufferRow = this->isSoftWrapHunk(hunk) ? hunk.old_end.row : hunk.old_end.row + 1;
  }

  while (lastScreenRow < endRow) {
    bufferRows.push_back(lastBufferRow);
    lastScreenRow++;
    lastBufferRow++;
  }

  return bufferRows;
}

double DisplayLayer::leadingWhitespaceLengthForSurroundingLines(double startBufferRow) {
  double length = 0;
  for (double bufferRow = startBufferRow - 1; bufferRow >= 0; bufferRow--) {
    auto line = this->buffer->lineForRow(bufferRow);
    if (line->size() > 0) {
      length = this->leadingWhitespaceLengthForNonEmptyLine(*line);
      break;
    }
  }

  const double lineCount = this->buffer->getLineCount();
  for (double bufferRow = startBufferRow + 1; bufferRow < lineCount; bufferRow++) {
    auto line = this->buffer->lineForRow(bufferRow);
    if (line->size() > 0) {
      length = std::max(length, this->leadingWhitespaceLengthForNonEmptyLine(*line));
      break;
    }
  }

  return length;
}

double DisplayLayer::leadingWhitespaceLengthForNonEmptyLine(const std::u16string &line) {
  double length = 0;
  for (double i = 0; i < line.size(); i++) {
    const char16_t character = line[i];
    if (character == u' ') {
      length++;
    } else if (character == u'\t') {
      length += this->tabLength - std::fmod(length, this->tabLength);
    } else {
      break;
    }
  }
  return length;
}

double DisplayLayer::findTrailingWhitespaceStartColumn(double bufferRow) {
  Point position;
  for (position = Point(bufferRow, this->buffer->lineLengthForRow(bufferRow) - 1); position.column >= 0; position.column--) {
    const char16_t previousCharacter = this->buffer->getCharacterAtPosition(position);
    if (previousCharacter != u' ' && previousCharacter != u'\t') {
      break;
    }
  }
  return position.column + 1;
}

int32_t DisplayLayer::registerBuiltInScope(int32_t flags, const std::string &className) {
  /*if (this.nextBuiltInScopeId > MAX_BUILT_IN_SCOPE_ID) {
    throw new Error('Built in scope ids exhausted')
  }*/

  int32_t scopeId;
  if (className.size() > 0) {
    scopeId = this->nextBuiltInScopeId += 2;
    this->builtInClassNamesByScopeId[scopeId] = className;
  } else {
    scopeId = 0;
  }
  this->builtInScopeIdsByFlags[flags] = scopeId;
  return scopeId;
}

int32_t DisplayLayer::getBuiltInScopeId(int32_t flags) {
  auto iter = this->builtInScopeIdsByFlags.find(flags);
  if (iter != this->builtInScopeIdsByFlags.end()) {
    return iter->second;
  } else {
    return -1;
  }
}

std::string DisplayLayer::classNameForScopeId(int32_t scopeId) {
  if (scopeId <= MAX_BUILT_IN_SCOPE_ID) {
    return this->builtInClassNamesByScopeId[scopeId];
  } else {
    return this->buffer->languageMode->classNameForScopeId(scopeId);
  }
}

int32_t DisplayLayer::scopeIdForTag(int32_t tag) const {
  if (this->isCloseTag(tag)) tag++;
  return -tag;
}

std::string DisplayLayer::classNameForTag(int32_t tag) {
  return this->classNameForScopeId(this->scopeIdForTag(tag));
}

int32_t DisplayLayer::openTagForScopeId(int32_t scopeId) {
  return -scopeId;
}

int32_t DisplayLayer::closeTagForScopeId(int32_t scopeId) {
  return -scopeId - 1;
}

bool DisplayLayer::isOpenTag(int32_t tag) const {
  return tag < 0 && (tag & 1) == 1;
}

bool DisplayLayer::isCloseTag(int32_t tag) const {
  return tag < 0 && (tag & 1) == 0;
}

void DisplayLayer::bufferWillChange(Range oldRange) {
  const double lineCount = this->buffer->getLineCount();
  double endRow = oldRange.end.row;
  while (endRow + 1 < lineCount && this->buffer->lineLengthForRow(endRow + 1) == 0) {
    endRow++;
  }
  this->populateSpatialIndexIfNeeded(endRow + 1, INFINITY);
}

void DisplayLayer::bufferDidChange(Range oldRange, Range newRange) {
  double startRow = oldRange.start.row;
  double oldEndRow = oldRange.end.row;
  double newEndRow = newRange.end.row;

  // Indent guides on sequences of blank lines are affected by the content of
  // adjacent lines.
  if (this->showIndentGuides) {
    while (startRow > 0) {
      if (this->buffer->lineLengthForRow(startRow - 1) > 0) break;
      startRow--;
    }

    while (newEndRow < this->buffer->getLastRow()) {
      if (this->buffer->lineLengthForRow(newEndRow + 1) > 0) break;
      oldEndRow++;
      newEndRow++;
    }
  }

  this->indexedBufferRowCount += newEndRow - oldEndRow;
  this->didChange(this->updateSpatialIndex(startRow, oldEndRow + 1, newEndRow + 1, INFINITY));
}

void DisplayLayer::didChange(UpdateResult updateResult) {
  this->changesSinceLastEvent.splice(updateResult.start, updateResult.oldExtent, updateResult.newExtent);
  if (this->buffer->transactCallDepth == 0) this->emitDeferredChangeEvents();
}

void DisplayLayer::emitDeferredChangeEvents() {
  if (this->changesSinceLastEvent.get_change_count() > 0) {
    /*this.emitter.emit('did-change', this.changesSinceLastEvent.getChanges().map((change) => {
      return {
        oldRange: new Range(change.oldStart, change.oldEnd),
        newRange: new Range(change.newStart, change.newEnd)
      }
    }));*/
    this->changesSinceLastEvent = Patch();
  }
}

DisplayLayer::UpdateResult DisplayLayer::updateSpatialIndex(double startBufferRow, double oldEndBufferRow, double newEndBufferRow, double endScreenRow /*, deadline = NullDeadline */) {
  const double originalOldEndBufferRow = oldEndBufferRow;
  startBufferRow = this->findBoundaryPrecedingBufferRow(startBufferRow);
  oldEndBufferRow = this->findBoundaryFollowingBufferRow(oldEndBufferRow);
  newEndBufferRow += (oldEndBufferRow - originalOldEndBufferRow);

  const double startScreenRow = this->translateBufferPositionWithSpatialIndex(Point(startBufferRow, 0), ClipDirection::backward).row;
  const double oldEndScreenRow = this->translateBufferPositionWithSpatialIndex(Point(oldEndBufferRow, 0), ClipDirection::backward).row;
  this->spatialIndex->splice_old(
    Point(startBufferRow, 0),
    Point(oldEndBufferRow - startBufferRow, 0),
    Point(newEndBufferRow - startBufferRow, 0)
  );

  auto folds = this->computeFoldsInBufferRowRange(startBufferRow, newEndBufferRow);

  std::vector<double> insertedScreenLineLengths;
  std::vector<double> insertedTabCounts;
  std::vector<double> currentScreenLineTabColumns;
  Point rightmostInsertedScreenPosition = Point(0, -1);
  double bufferRow = startBufferRow;
  double screenRow = startScreenRow;
  double bufferColumn = 0;
  double unexpandedScreenColumn = 0;
  double expandedScreenColumn = 0;

  while (true) {
    if (bufferRow >= newEndBufferRow) break;
    if (screenRow >= endScreenRow && bufferColumn == 0) break;
    //if (deadline.timeRemaining() < 2) break;
    auto bufferLine = this->buffer->lineForRow(bufferRow);
    if (!bufferLine) break;
    double bufferLineLength = bufferLine->size();
    currentScreenLineTabColumns.resize(0);
    double screenLineWidth = 0;
    double lastWrapBoundaryUnexpandedScreenColumn = 0;
    double lastWrapBoundaryExpandedScreenColumn = 0;
    double lastWrapBoundaryScreenLineWidth = 0;
    double firstNonWhitespaceScreenColumn = -1;

    while (bufferColumn <= bufferLineLength) {
      const optional<Point> foldEnd = folds.count(bufferRow) && folds[bufferRow].count(bufferColumn) ? folds[bufferRow][bufferColumn] : optional<Point>();
      const optional<char16_t> previousCharacter = bufferColumn >= 1 ? optional<char16_t>((*bufferLine)[bufferColumn - 1]) : optional<char16_t>();
      const optional<char16_t> character = foldEnd ? optional<char16_t>(this->foldCharacter) : bufferColumn <= bufferLineLength - 1 ? optional<char16_t>((*bufferLine)[bufferColumn]) : optional<char16_t>();

      // Are we in leading whitespace? If yes, record the *end* of the leading
      // whitespace if we've reached a non whitespace character. If no, record
      // the current column if it is a viable soft wrap boundary.
      if (firstNonWhitespaceScreenColumn < 0) {
        if (!character || *character != u' ' && *character != u'\t') {
          firstNonWhitespaceScreenColumn = expandedScreenColumn;
        }
      } else {
        if (previousCharacter &&
            character &&
            this->isWrapBoundary(*previousCharacter, *character)) {
          lastWrapBoundaryUnexpandedScreenColumn = unexpandedScreenColumn;
          lastWrapBoundaryExpandedScreenColumn = expandedScreenColumn;
          lastWrapBoundaryScreenLineWidth = screenLineWidth;
        }
      }

      // Determine the on-screen width of the character for soft-wrap calculations
      double characterWidth;
      if (character && *character == u'\t') {
        const double distanceToNextTabStop = this->tabLength - std::fmod(expandedScreenColumn, this->tabLength);
        characterWidth = this->ratioForCharacter(' ') * distanceToNextTabStop;
      } else if (character) {
        characterWidth = this->ratioForCharacter(*character);
      } else {
        characterWidth = 0;
      }

      const bool insertSoftLineBreak =
        screenLineWidth > 0 && characterWidth > 0 &&
        screenLineWidth + characterWidth > this->softWrapColumn &&
        previousCharacter && character &&
        !isCharacterPair(*previousCharacter, *character);

      if (insertSoftLineBreak) {
        double indentLength = (firstNonWhitespaceScreenColumn < this->softWrapColumn)
          ? std::max(0.0, firstNonWhitespaceScreenColumn)
          : 0;
        if (indentLength + this->softWrapHangingIndent < this->softWrapColumn) {
          indentLength += this->softWrapHangingIndent;
        }

        const double unexpandedWrapColumn = lastWrapBoundaryUnexpandedScreenColumn ? lastWrapBoundaryUnexpandedScreenColumn : unexpandedScreenColumn;
        const double expandedWrapColumn = lastWrapBoundaryExpandedScreenColumn ? lastWrapBoundaryExpandedScreenColumn : expandedScreenColumn;
        const double wrapWidth = lastWrapBoundaryScreenLineWidth ? lastWrapBoundaryScreenLineWidth : screenLineWidth;
        this->spatialIndex->splice(
          Point(screenRow, unexpandedWrapColumn),
          Point::ZERO,
          Point(1, indentLength)
        );

        insertedScreenLineLengths.push_back(expandedWrapColumn);
        if (expandedWrapColumn > rightmostInsertedScreenPosition.column) {
          rightmostInsertedScreenPosition.row = screenRow;
          rightmostInsertedScreenPosition.column = expandedWrapColumn;
        }
        screenRow++;

        // To determine the expanded screen column following the wrap, we need
        // to re-expand each tab following the wrap boundary, because tabs may
        // take on different lengths due to starting at different screen columns.
        double unexpandedScreenColumnAfterLastTab = indentLength;
        double expandedScreenColumnAfterLastTab = indentLength;
        double tabCountPrecedingWrap = 0;
        for (double i = 0; i < currentScreenLineTabColumns.size(); i++) {
          const double tabColumn = currentScreenLineTabColumns[i];
          if (tabColumn < unexpandedWrapColumn) {
            tabCountPrecedingWrap++;
          } else {
            const double tabColumnAfterWrap = indentLength + tabColumn - unexpandedWrapColumn;
            expandedScreenColumnAfterLastTab += (tabColumnAfterWrap - unexpandedScreenColumnAfterLastTab);
            expandedScreenColumnAfterLastTab += this->tabLength - std::fmod(expandedScreenColumnAfterLastTab, this->tabLength);
            unexpandedScreenColumnAfterLastTab = tabColumnAfterWrap + 1;
            currentScreenLineTabColumns[i - tabCountPrecedingWrap] = tabColumnAfterWrap;
          }
        }
        insertedTabCounts.push_back(tabCountPrecedingWrap);
        //currentScreenLineTabColumns.length -= tabCountPrecedingWrap;
        currentScreenLineTabColumns.resize(currentScreenLineTabColumns.size() - tabCountPrecedingWrap);

        unexpandedScreenColumn = unexpandedScreenColumn - unexpandedWrapColumn + indentLength;
        expandedScreenColumn = expandedScreenColumnAfterLastTab + unexpandedScreenColumn - unexpandedScreenColumnAfterLastTab;
        screenLineWidth = (indentLength * this->ratioForCharacter(' ')) + (screenLineWidth - wrapWidth);

        lastWrapBoundaryUnexpandedScreenColumn = 0;
        lastWrapBoundaryExpandedScreenColumn = 0;
        lastWrapBoundaryScreenLineWidth = 0;
      }

      // If there is a fold at this position, splice it into the spatial index
      // and jump to the end of the fold.
      if (foldEnd) {
        this->spatialIndex->splice(
          Point(screenRow, unexpandedScreenColumn),
          traversal(*foldEnd, Point(bufferRow, bufferColumn)),
          Point(0, 1)
        );
        unexpandedScreenColumn++;
        expandedScreenColumn++;
        screenLineWidth += characterWidth;
        bufferRow = foldEnd->row;
        bufferColumn = foldEnd->column;
        bufferLine = this->buffer->lineForRow(bufferRow);
        bufferLineLength = bufferLine->size();
      } else {
        // If there is no fold at this position, check if we need to handle
        // a hard tab at this position and advance by a single buffer column.
        if (character && *character == u'\t') {
          currentScreenLineTabColumns.push_back(unexpandedScreenColumn);
          const double distanceToNextTabStop = this->tabLength - std::fmod(expandedScreenColumn, this->tabLength);
          expandedScreenColumn += distanceToNextTabStop;
          screenLineWidth += distanceToNextTabStop * this->ratioForCharacter(' ');
        } else {
          expandedScreenColumn++;
          screenLineWidth += characterWidth;
        }
        unexpandedScreenColumn++;
        bufferColumn++;
      }
    }

    expandedScreenColumn--;
    insertedScreenLineLengths.push_back(expandedScreenColumn);
    insertedTabCounts.push_back(currentScreenLineTabColumns.size());
    if (expandedScreenColumn > rightmostInsertedScreenPosition.column) {
      rightmostInsertedScreenPosition.row = screenRow;
      rightmostInsertedScreenPosition.column = expandedScreenColumn;
    }

    bufferRow++;
    bufferColumn = 0;

    screenRow++;
    unexpandedScreenColumn = 0;
    expandedScreenColumn = 0;
  }

  if (bufferRow > this->indexedBufferRowCount) {
    this->indexedBufferRowCount = bufferRow;
    if (bufferRow == this->buffer->getLineCount()) {
      this->spatialIndex->rebalance();
    }
  }

  const double oldScreenRowCount = oldEndScreenRow - startScreenRow;
  spliceArray(
    this->screenLineLengths,
    startScreenRow,
    oldScreenRowCount,
    insertedScreenLineLengths
  );
  spliceArray(
    this->tabCounts,
    startScreenRow,
    oldScreenRowCount,
    insertedTabCounts
  );

  const double lastRemovedScreenRow = startScreenRow + oldScreenRowCount;
  if (rightmostInsertedScreenPosition.column > this->rightmostScreenPosition.column) {
    this->rightmostScreenPosition = rightmostInsertedScreenPosition;
  } else if (lastRemovedScreenRow < this->rightmostScreenPosition.row) {
    this->rightmostScreenPosition.row += insertedScreenLineLengths.size() - oldScreenRowCount;
  } else if (startScreenRow <= this->rightmostScreenPosition.row) {
    this->rightmostScreenPosition = Point(0, 0);
    for (double row = 0, rowCount = this->screenLineLengths.size(); row < rowCount; row++) {
      if (this->screenLineLengths[row] > this->rightmostScreenPosition.column) {
        this->rightmostScreenPosition.row = row;
        this->rightmostScreenPosition.column = this->screenLineLengths[row];
      }
    }
  }

  spliceArray(
    this->cachedScreenLines,
    startScreenRow,
    oldScreenRowCount,
    std::vector<optional<ScreenLine>>(insertedScreenLineLengths.size())
  );

  return {
    Point(startScreenRow, 0),
    Point(oldScreenRowCount, 0),
    Point(insertedScreenLineLengths.size(), 0)
  };
}

void DisplayLayer::populateSpatialIndexIfNeeded(double endBufferRow, double endScreenRow) {
  endBufferRow = std::min(this->buffer->getLineCount(), endBufferRow);
  if (endBufferRow > this->indexedBufferRowCount && endScreenRow > this->screenLineLengths.size()) {
    this->updateSpatialIndex(
      this->indexedBufferRowCount,
      endBufferRow,
      endBufferRow,
      endScreenRow
    );
  }
}

double DisplayLayer::findBoundaryPrecedingBufferRow(double bufferRow) {
  while (true) {
    if (bufferRow == 0) return 0;
    Point screenPosition = this->translateBufferPositionWithSpatialIndex(Point(bufferRow, 0), ClipDirection::backward);
    Point bufferPosition = this->translateScreenPositionWithSpatialIndex(Point(screenPosition.row, 0), ClipDirection::backward, false);
    if (screenPosition.column == 0 && bufferPosition.column == 0) {
      return bufferPosition.row;
    } else {
      bufferRow = bufferPosition.row;
    }
  }
}

double DisplayLayer::findBoundaryFollowingBufferRow(double bufferRow) {
  while (true) {
    Point screenPosition = this->translateBufferPositionWithSpatialIndex(Point(bufferRow, 0), ClipDirection::forward);
    if (screenPosition.column == 0) {
      return bufferRow;
    } else {
      const Point endOfScreenRow = Point(
        screenPosition.row,
        this->screenLineLengths[screenPosition.row]
      );
      bufferRow = this->translateScreenPositionWithSpatialIndex(endOfScreenRow, ClipDirection::forward, false).row + 1;
    }
  }
}

std::pair<double, double> DisplayLayer::findBoundaryFollowingScreenRow(double screenRow) {
  while (true) {
    Point bufferPosition = this->translateScreenPositionWithSpatialIndex(Point(screenRow, 0), ClipDirection::forward);
    if (bufferPosition.column == 0) {
      return std::make_pair(bufferPosition.row, screenRow);
    } else {
      const Point endOfBufferRow = Point(
        bufferPosition.row,
        this->buffer->lineLengthForRow(bufferPosition.row)
      );
      screenRow = this->translateBufferPositionWithSpatialIndex(endOfBufferRow, ClipDirection::forward).row + 1;
    }
  }
}

// Returns a map describing fold starts and ends, structured as
// fold start row -> fold start column -> fold end point
std::unordered_map<double, std::unordered_map<double, Point>> DisplayLayer::computeFoldsInBufferRowRange(double startBufferRow, double endBufferRow) {
  std::unordered_map<double, std::unordered_map<double, Point>> folds;
  auto foldMarkers = this->foldsMarkerLayer->findMarkers({
    intersectsRowRange(startBufferRow, endBufferRow - 1)
  });

  // If the given buffer range exceeds the indexed range, we need to ensure
  // we consider any folds that intersect the combined row range of the
  // initially-queried folds, since we couldn't use the index to expand the
  // row range to account for these extra folds ahead of time.
  if (endBufferRow >= this->indexedBufferRowCount) {
    for (double i = 0; i < foldMarkers.size(); i++) {
      const auto& marker = foldMarkers[i];
      const auto& nextMarker = foldMarkers[i + 1];
      if (marker->getEndPosition().row >= endBufferRow &&
          (!(i + 1 < foldMarkers.size()) || nextMarker->getEndPosition().row < marker->getEndPosition().row)) {
        const auto intersectingMarkers = this->foldsMarkerLayer->findMarkers({
          intersectsRow(marker->getEndPosition().row)
        });
        endBufferRow = marker->getEndPosition().row + 1;
        foldMarkers.erase(foldMarkers.begin() + i, foldMarkers.end());
        foldMarkers.insert(foldMarkers.end(), intersectingMarkers.begin(), intersectingMarkers.end());
      }
    }
  }

  for (double i = 0; i < foldMarkers.size(); i++) {
    const Point foldStart = foldMarkers[i]->getStartPosition();
    Point foldEnd = foldMarkers[i]->getEndPosition();

    // Merge overlapping folds
    while (i < foldMarkers.size() - 1) {
      const auto& nextFoldMarker = foldMarkers[i + 1];
      if (compare(nextFoldMarker->getStartPosition(), foldEnd) < 0) {
        if (compare(foldEnd, nextFoldMarker->getEndPosition()) < 0) {
          foldEnd = nextFoldMarker->getEndPosition();
        }
        i++;
      } else {
        break;
      }
    }

    // Add non-empty folds to the returned result
    if (compare(foldStart, foldEnd) < 0) {
      if (!folds.count(foldStart.row)) folds[foldStart.row] = std::unordered_map<double, Point>();
      folds[foldStart.row][foldStart.column] = foldEnd;
    }
  }

  return folds;
}

bool DisplayLayer::isSoftWrapHunk(const Patch::Change &hunk) {
  return isEqual(hunk.old_start, hunk.old_end);
}

static bool isWordStart(char16_t previousCharacter, char16_t character) {
  return (previousCharacter == u' ' || previousCharacter == u'\t') &&
    (character != u' ' && character != u'\t');
}

static double unitRatio(char16_t c) {
  return 1;
}
