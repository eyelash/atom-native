#include "text-editor.h"
#include "cursor.h"
#include "selection.h"
#include "decoration-manager.h"
#include <text-buffer.h>
#include <display-marker-layer.h>
#include <display-marker.h>
#include <helpers.h>
#include <set>

constexpr const char16_t *DEFAULT_NON_WORD_CHARACTERS = u"/\\()\"':,.;<>~!@#$%^&*|+=[]{}`?-…";

TextEditor::TextEditor() {
  this->softTabs = true;
  this->buffer = new TextBuffer();
  this->displayLayer = this->buffer->addDisplayLayer();
  //this->defaultMarkerLayer = this->displayLayer->addMarkerLayer();
  this->selectionsMarkerLayer = this->addMarkerLayer();
  this->decorationManager = new DecorationManager(this);
  this->decorateMarkerLayer(this->selectionsMarkerLayer, { Decoration::Type::cursor });
  this->decorateCursorLine();

  this->subscribeToBuffer();
  this->subscribeToDisplayLayer();

  this->addCursorAtBufferPosition(Point(0, 0));
}

TextEditor::~TextEditor() {
  for (Cursor *cursor : this->cursors) {
    delete cursor;
  }
  for (Selection *selection : this->selections) {
    delete selection;
  }
  delete this->buffer;
  delete this->decorationManager;
}

void TextEditor::decorateCursorLine() {
  //this.cursorLineDecorations = [
    this->decorateMarkerLayer(this->selectionsMarkerLayer, []() {
      Decoration::Properties properties;
      properties.type = Decoration::Type::line;
      properties.class_ = "cursor-line";
      properties.onlyEmpty = true;
      return properties;
    }());
    this->decorateMarkerLayer(this->selectionsMarkerLayer, {
      Decoration::Type::line_number,
      "cursor-line"
    });
    this->decorateMarkerLayer(this->selectionsMarkerLayer, []() {
      Decoration::Properties properties;
      properties.type = Decoration::Type::line_number;
      properties.class_ = "cursor-line-no-selection";
      properties.onlyHead = true;
      properties.onlyEmpty = true;
      return properties;
    }());
  //];
}

void TextEditor::subscribeToBuffer() {

}

void TextEditor::subscribeToDisplayLayer() {
  using namespace std::placeholders;
  this->selectionsMarkerLayer->onDidCreateMarker(std::bind(&TextEditor::addSelection, this, _1));
}

/*
Section: Event Subscription
*/

/*
Section: Buffer
*/

TextBuffer *TextEditor::getBuffer() {
  return this->buffer;
}

/*
Section: File Details
*/

bool TextEditor::isModified() {
  return this->buffer->isModified();
}

bool TextEditor::isEmpty() {
  return this->buffer->isEmpty();
}

/*
Section: File Operations
*/

/*
Section: Reading Text
*/

std::u16string TextEditor::getText() {
  return this->buffer->getText();
}

std::u16string TextEditor::getTextInBufferRange(Range range) {
  return this->buffer->getTextInRange(range);
}

double TextEditor::getLineCount() {
  return this->buffer->getLineCount();
}

double TextEditor::getScreenLineCount() {
  return this->displayLayer->getScreenLineCount();
}

double TextEditor::getApproximateScreenLineCount() {
  return this->displayLayer->getApproximateScreenLineCount();
}

double TextEditor::getLastBufferRow() {
  return this->buffer->getLastRow();
}

double TextEditor::getLastScreenRow() {
  return this->getScreenLineCount() - 1;
}

optional<std::u16string> TextEditor::lineTextForBufferRow(double bufferRow) {
  return this->buffer->lineForRow(bufferRow);
}

std::u16string TextEditor::lineTextForScreenRow(double screenRow) {
  auto screenLine = this->screenLineForScreenRow(screenRow);
  /* if (screenLine) */ return screenLine.lineText;
}

DisplayLayer::ScreenLine TextEditor::screenLineForScreenRow(double screenRow) {
  return this->displayLayer->getScreenLine(screenRow);
}

double TextEditor::bufferRowForScreenRow(double screenRow) {
  return this->displayLayer->translateScreenPosition(Point(screenRow, 0)).row;
}

std::vector<double> TextEditor::bufferRowsForScreenRows(double startScreenRow, double endScreenRow) {
  return this->displayLayer->bufferRowsForScreenRows(
    startScreenRow,
    endScreenRow + 1
  );
}

double TextEditor::screenRowForBufferRow(double row) {
  return this->displayLayer->translateBufferPosition(Point(row, 0)).row;
}

Point TextEditor::getRightmostScreenPosition() {
  return this->displayLayer->getRightmostScreenPosition();
}

/*getApproximateRightmostScreenPosition() {
  return this.displayLayer.getApproximateRightmostScreenPosition();
}*/

double TextEditor::getMaxScreenLineLength() {
  return this->getRightmostScreenPosition().column;
}

double TextEditor::getLongestScreenRow() {
  return this->getRightmostScreenPosition().row;
}

/*getApproximateLongestScreenRow() {
  return this.getApproximateRightmostScreenPosition().row;
}*/

double TextEditor::lineLengthForScreenRow(double screenRow) {
  return this->displayLayer->lineLengthForScreenRow(screenRow);
}

Range TextEditor::bufferRangeForBufferRow(double row, bool includeNewline) {
  return this->buffer->rangeForRow(row, includeNewline);
}

std::u16string TextEditor::getTextInRange(Range range) {
  return this->buffer->getTextInRange(range);
}

bool TextEditor::isBufferRowBlank(double bufferRow) {
  return this->buffer->isRowBlank(bufferRow);
}

optional<double> TextEditor::nextNonBlankBufferRow(double bufferRow) {
  return this->buffer->nextNonBlankRow(bufferRow);
}

Point TextEditor::getEofBufferPosition() {
  return this->buffer->getEndPosition();
}

/*getCurrentParagraphBufferRange() {
  return this->getLastCursor().getCurrentParagraphBufferRange();
}*/

/*
Section: Mutating Text
*/

void TextEditor::setText(std::u16string &&text) {
  this->buffer->setText(std::move(text));
}

Range TextEditor::setTextInBufferRange(Range range, std::u16string &&text) {
  return this->getBuffer()->setTextInRange(range, std::move(text));
}

void TextEditor::insertText(const std::u16string &text) {
  this->mutateSelectedText([&](Selection *selection) {
    selection->insertText(text);
  });
}

void TextEditor::insertNewline() {
  return this->insertText(u"\n");
}

void TextEditor::delete_() {
  //if (!this->ensureWritable('delete', options)) return;
  return this->mutateSelectedText([&](Selection *selection) { selection->delete_(); });
}

void TextEditor::backspace() {
  //if (!this->ensureWritable('backspace', options)) return;
  return this->mutateSelectedText([&](Selection *selection) { selection->backspace(); });
}

void TextEditor::mutateSelectedText(std::function<void(Selection *)> fn /* , groupingInterval = 0 */ ) {
  return this->mergeIntersectingSelections([&]() {
    return this->transact(/* groupingInterval, */ [&]() {
      for (Selection *selection : this->getSelectionsOrderedByBufferPosition()) fn(selection);
    });
  });
}

void TextEditor::moveLineUp(/* options = {} */) {
  //if (!this.ensureWritable('moveLineUp', options)) return;

  auto selections = this->getSelectedBufferRanges();
  std::sort(selections.begin(), selections.end(), [](Range a, Range b) {
    return a.compare(b) < 0;
  });

  if (selections[0].start.row == 0) return;
  if (
    selections[selections.size() - 1].start.row == this->getLastBufferRow() &&
    *this->buffer->getLastLine() == u""
  )
    return;

  //this.transact(() => {
    std::vector<Range> newSelectionRanges;

    while (selections.size() > 0) {
      // Find selections spanning a contiguous set of lines
      Range selection = selections.front();
      selections.erase(selections.begin());
      std::vector<Range> selectionsToMove;
      selectionsToMove.push_back(selection);

      while (
        selections.size() > 0 &&
        selection.end.row == selections[0].start.row
      ) {
        selectionsToMove.push_back(selections[0]);
        selection.end.row = selections[0].end.row;
        selections.erase(selections.begin());
      }

      // Compute the buffer range spanned by all these selections, expanding it
      // so that it includes any folded region that intersects them.
      double startRow = selection.start.row;
      double endRow = selection.end.row;
      if (
        selection.end.row > selection.start.row &&
        selection.end.column == 0
      ) {
        // Don't move the last line of a multi-line selection if the selection ends at column 0
        endRow--;
      }

      startRow = this->displayLayer->findBoundaryPrecedingBufferRow(startRow);
      endRow = this->displayLayer->findBoundaryFollowingBufferRow(endRow + 1);
      const Range linesRange = Range(Point(startRow, 0), Point(endRow, 0));

      // If selected line range is preceded by a fold, one line above on screen
      // could be multiple lines in the buffer.
      const double precedingRow = this->displayLayer->findBoundaryPrecedingBufferRow(
        startRow - 1
      );
      const double insertDelta = linesRange.start.row - precedingRow;

      // Any folds in the text that is moved will need to be re-created.
      // It includes the folds that were intersecting with the selection.
      /*const rangesToRefold = this->displayLayer
        .destroyFoldsIntersectingBufferRange(linesRange)
        .map(range => range.translate([-insertDelta, 0]));*/

      // Delete lines spanned by selection and insert them on the preceding buffer row
      std::u16string lines = this->buffer->getTextInRange(linesRange);
      if (lines[lines.size() - 1] != u'\n') {
        lines += this->buffer->lineEndingForRow(linesRange.end.row - 2);
      }
      this->buffer->delete_(linesRange);
      this->buffer->insert({precedingRow, 0}, std::move(lines));

      // Restore folds that existed before the lines were moved
      /*for (let rangeToRefold of rangesToRefold) {
        this->displayLayer.foldBufferRange(rangeToRefold);
      }*/

      for (const Range selectionToMove : selectionsToMove) {
        newSelectionRanges.push_back(selectionToMove.translate({-insertDelta, 0}));
      }
    }

    this->setSelectedBufferRanges(newSelectionRanges /* , {
      autoscroll: false,
      preserveFolds: true
    } */);
    //if (this->shouldAutoIndent()) this->autoIndentSelectedRows();
    //this->scrollToBufferPosition([newSelectionRanges[0].start.row, 0]);
  //});
}

void TextEditor::moveLineDown(/* options = {} */) {
  //if (!this.ensureWritable('moveLineDown', options)) return;

  auto selections = this->getSelectedBufferRanges();
  std::sort(selections.begin(), selections.end(), [](Range a, Range b) { return b.compare(a) < 0; });

  //this.transact(() => {
    this->consolidateSelections();
    std::vector<Range> newSelectionRanges;

    while (selections.size() > 0) {
      // Find selections spanning a contiguous set of lines
      Range selection = selections.front();
      selections.erase(selections.begin());

      std::vector<Range> selectionsToMove;
      selectionsToMove.push_back(selection);

      // if the current selection start row matches the next selections' end row - make them one selection
      while (
        selections.size() > 0 &&
        selection.start.row == selections[0].end.row
      ) {
        selectionsToMove.push_back(selections[0]);
        selection.start.row = selections[0].start.row;
        selections.erase(selections.begin());
      }

      // Compute the buffer range spanned by all these selections, expanding it
      // so that it includes any folded region that intersects them.
      double startRow = selection.start.row;
      double endRow = selection.end.row;
      if (
        selection.end.row > selection.start.row &&
        selection.end.column == 0
      ) {
        // Don't move the last line of a multi-line selection if the selection ends at column 0
        endRow--;
      }

      startRow = this->displayLayer->findBoundaryPrecedingBufferRow(startRow);
      endRow = this->displayLayer->findBoundaryFollowingBufferRow(endRow + 1);
      const Range linesRange = Range(Point(startRow, 0), Point(endRow, 0));

      // If selected line range is followed by a fold, one line below on screen
      // could be multiple lines in the buffer. But at the same time, if the
      // next buffer row is wrapped, one line in the buffer can represent many
      // screen rows.
      const double followingRow = std::min(
        this->buffer->getLineCount(),
        this->displayLayer->findBoundaryFollowingBufferRow(endRow + 1)
      );
      const double insertDelta = followingRow - linesRange.end.row;

      // Any folds in the text that is moved will need to be re-created.
      // It includes the folds that were intersecting with the selection.
      /*const rangesToRefold = this->displayLayer
        .destroyFoldsIntersectingBufferRange(linesRange)
        .map(range => range.translate([insertDelta, 0]));*/

      // Delete lines spanned by selection and insert them on the following correct buffer row
      std::u16string lines = this->buffer->getTextInRange(linesRange);
      if (followingRow - 1 == this->buffer->getLastRow()) {
        lines = u"\n" + lines;
      }

      this->buffer->insert({followingRow, 0}, std::move(lines));
      this->buffer->delete_(linesRange);

      // Restore folds that existed before the lines were moved
      /*for (let rangeToRefold of rangesToRefold) {
        this->displayLayer->foldBufferRange(rangeToRefold);
      }*/

      for (const Range selectionToMove : selectionsToMove) {
        newSelectionRanges.push_back(selectionToMove.translate({insertDelta, 0}));
      }
    }

    this->setSelectedBufferRanges(newSelectionRanges /* , {
      autoscroll: false,
      preserveFolds: true
    } */);
    //if (this.shouldAutoIndent()) this.autoIndentSelectedRows();
    //this.scrollToBufferPosition([newSelectionRanges[0].start.row - 1, 0]);
  //});
}

void TextEditor::duplicateLines(/* options = {} */) {
  //if (!this.ensureWritable('duplicateLines', options)) return;
  //this.transact(() => {
    const auto selections = this->getSelectionsOrderedByBufferPosition();
    std::vector<Range> previousSelectionRanges(selections.size());

    double i = selections.size() - 1.0;
    while (i >= 0) {
      const double j = i;
      previousSelectionRanges[i] = selections[i]->getBufferRange();
      if (selections[i]->isEmpty()) {
        const Point start = selections[i]->getScreenRange().start;
        selections[i]->setScreenRange({{start.row, 0}, {start.row + 1, 0}} /* , {
          preserveFolds: true
        } */);
      }
      const auto bufferRowRange = selections[i]->getBufferRowRange();
      double startRow = bufferRowRange.first, endRow = bufferRowRange.second;
      endRow++;
      while (i > 0) {
        const auto previousBufferRowRange = selections[i - 1]->getBufferRowRange();
        const double previousSelectionStartRow = previousBufferRowRange.first;
        const double previousSelectionEndRow = previousBufferRowRange.second;
        if (previousSelectionEndRow == startRow) {
          startRow = previousSelectionStartRow;
          previousSelectionRanges[i - 1] = selections[i - 1]->getBufferRange();
          i--;
        } else {
          break;
        }
      }

      /*const intersectingFolds = this.displayLayer.foldsIntersectingBufferRange(
        [[startRow, 0], [endRow, 0]]
      );*/
      auto textToDuplicate = this->getTextInBufferRange({
        {startRow, 0},
        {endRow, 0}
      });
      if (endRow > this->getLastBufferRow())
        textToDuplicate = u"\n" + textToDuplicate;
      this->buffer->insert({endRow, 0}, std::move(textToDuplicate));

      const double insertedRowCount = endRow - startRow;

      for (double k = i; k <= j; k++) {
        selections[k]->setBufferRange(
          previousSelectionRanges[k].translate({insertedRowCount, 0})
        );
      }

      /*for (const fold of intersectingFolds) {
        const foldRange = this.displayLayer.bufferRangeForFold(fold);
        this.displayLayer.foldBufferRange(
          foldRange.translate([insertedRowCount, 0])
        );
      }*/

      i--;
    }
  //});
}

void TextEditor::splitSelectionsIntoLines() {
  this->mergeIntersectingSelections([&]() {
    for (Selection *selection : this->getSelections()) {
      const Range range = selection->getBufferRange();
      if (range.isSingleLine()) continue;

      const Point start = range.start;
      const Point end = range.end;
      this->addSelectionForBufferRange({start, {start.row, INFINITY}});
      double row = start.row;
      while (++row < end.row) {
        this->addSelectionForBufferRange({{row, 0}, {row, INFINITY}});
      }
      if (end.column != 0)
        this->addSelectionForBufferRange({
          {end.row, 0},
          {end.row, end.column}
        });
      selection->destroy();
    }
  });
}

void TextEditor::insertNewlineBelow(/* options = {} */) {
  //if (!this->ensureWritable('insertNewlineBelow', options)) return;
  //this->transact(() => {
    this->moveToEndOfLine();
    this->insertNewline(/* options */);
  //});
}

void TextEditor::insertNewlineAbove(/* options = {} */) {
  //if (!this->ensureWritable('insertNewlineAbove', options)) return;
  //this->transact(() => {
    const double bufferRow = this->getCursorBufferPosition().row;
    const double indentLevel = this->indentationForBufferRow(bufferRow);
    const bool onFirstLine = bufferRow == 0;

    this->moveToBeginningOfLine();
    this->moveLeft();
    this->insertNewline(/* options */);

    if (
      this->shouldAutoIndent() &&
      this->indentationForBufferRow(bufferRow) < indentLevel
    ) {
      this->setIndentationForBufferRow(bufferRow, indentLevel);
    }

    if (onFirstLine) {
      this->moveUp();
      this->moveToEndOfLine();
    }
  //});
}

void TextEditor::deleteToBeginningOfWord(/* options = {} */) {
  //if (!this.ensureWritable('deleteToBeginningOfWord', options)) return;
  this->mutateSelectedText([](Selection *selection) {
    selection->deleteToBeginningOfWord(/* options */);
  });
}

void TextEditor::deleteToPreviousWordBoundary(/* options = {} */) {
  //if (!this.ensureWritable('deleteToPreviousWordBoundary', options)) return;
  this->mutateSelectedText([](Selection *selection) {
    selection->deleteToPreviousWordBoundary(/* options */);
  });
}

void TextEditor::deleteToNextWordBoundary(/* options = {} */) {
  //if (!this.ensureWritable('deleteToNextWordBoundary', options)) return;
  this->mutateSelectedText([](Selection *selection) {
    selection->deleteToNextWordBoundary(/* options */);
  });
}

void TextEditor::deleteToBeginningOfSubword(/* options = {} */) {
  //if (!this.ensureWritable('deleteToBeginningOfSubword', options)) return;
  this->mutateSelectedText([](Selection *selection) {
    selection->deleteToBeginningOfSubword(/* options */);
  });
}

void TextEditor::deleteToEndOfSubword(/* options = {} */) {
  //if (!this.ensureWritable('deleteToEndOfSubword', options)) return;
  this->mutateSelectedText([](Selection *selection) {
    selection->deleteToEndOfSubword(/* options */);
  });
}

void TextEditor::deleteToBeginningOfLine(/* options = {} */) {
  //if (!this.ensureWritable('deleteToBeginningOfLine', options)) return;
  this->mutateSelectedText([](Selection *selection) {
    selection->deleteToBeginningOfLine(/* options */);
  });
}

void TextEditor::deleteToEndOfLine(/* options = {} */) {
  //if (!this.ensureWritable('deleteToEndOfLine', options)) return;
  this->mutateSelectedText([](Selection *selection) { selection->deleteToEndOfLine(/* options */); });
}

void TextEditor::deleteToEndOfWord(/* options = {} */) {
  //if (!this.ensureWritable('deleteToEndOfWord', options)) return;
  this->mutateSelectedText([](Selection *selection) { selection->deleteToEndOfWord(/* options */); });
}

void TextEditor::deleteLine(/* options = {} */) {
  //if (!this.ensureWritable('deleteLine', options)) return;
  this->mergeSelectionsOnSameRows();
  this->mutateSelectedText([](Selection *selection) { selection->deleteLine(/* options */); });
}

/*
Section: History
*/

void TextEditor::transact(std::function<void()> fn) {
  return this->buffer->transact(fn);
}

/*
Section: TextEditor Coordinates
*/

Point TextEditor::screenPositionForBufferPosition(Point bufferPosition) {
  return this->displayLayer->translateBufferPosition(bufferPosition);
}

Point TextEditor::bufferPositionForScreenPosition(Point screenPosition) {
  return this->displayLayer->translateScreenPosition(screenPosition);
}

Range TextEditor::screenRangeForBufferRange(Range bufferRange) {
  const Point start = this->screenPositionForBufferPosition(
    bufferRange.start
  );
  const Point end = this->screenPositionForBufferPosition(bufferRange.end);
  return Range(start, end);
}

Range TextEditor::bufferRangeForScreenRange(Range screenRange) {
  const Point start = this->bufferPositionForScreenPosition(screenRange.start);
  const Point end = this->bufferPositionForScreenPosition(screenRange.end);
  return Range(start, end);
}

Point TextEditor::clipBufferPosition(Point bufferPosition) {
  return this->buffer->clipPosition(bufferPosition);
}

Range TextEditor::clipBufferRange(Range range) {
  return this->buffer->clipRange(range);
}

Point TextEditor::clipScreenPosition(Point screenPosition) {
  return this->displayLayer->clipScreenPosition(screenPosition);
}

Range TextEditor::clipScreenRange(Range screenRange) {
  const Point start = this->displayLayer->clipScreenPosition(
    screenRange.start
  );
  const Point end = this->displayLayer->clipScreenPosition(screenRange.end);
  return Range(start, end);
}

/*
Section: Decorations
*/

Decoration *TextEditor::decorateMarker(DisplayMarker *marker, Decoration::Properties decorationParams) {
  return this->decorationManager->decorateMarker(marker, decorationParams);
}

LayerDecoration *TextEditor::decorateMarkerLayer(DisplayMarkerLayer *markerLayer, Decoration::Properties decorationParams) {
  return this->decorationManager->decorateMarkerLayer(
    markerLayer,
    decorationParams
  );
}

/*
Section: Markers
*/

DisplayMarkerLayer *TextEditor::addMarkerLayer() {
  return this->displayLayer->addMarkerLayer();
}

/*
Section: Cursors
*/

Point TextEditor::getCursorBufferPosition() {
  return this->getLastCursor()->getBufferPosition();
}

void TextEditor::setCursorBufferPosition(Point position) {
  return this->moveCursors([&](Cursor *cursor) {
    cursor->setBufferPosition(position);
  });
}

Point TextEditor::getCursorScreenPosition() {
  return this->getLastCursor()->getScreenPosition();
}

void TextEditor::setCursorScreenPosition(Point position) {
  this->moveCursors([&](Cursor *cursor) {
    cursor->setScreenPosition(position);
  });
}

Cursor *TextEditor::addCursorAtBufferPosition(Point bufferPosition) {
  this->selectionsMarkerLayer->markBufferPosition(bufferPosition /* , {
    invalidate: 'never'
  } */ );
  return this->getLastSelection()->cursor;
}

Cursor *TextEditor::addCursorAtScreenPosition(Point screenPosition) {
  this->selectionsMarkerLayer->markScreenPosition(screenPosition /* , {
    invalidate: 'never'
  } */ );
  return this->getLastSelection()->cursor;
}

bool TextEditor::hasMultipleCursors() {
  return this->getCursors().size() > 1;
}

void TextEditor::moveUp(double lineCount) {
  return this->moveCursors([&](Cursor *cursor) {
    cursor->moveUp(lineCount, true);
  });
}

void TextEditor::moveDown(double lineCount) {
  return this->moveCursors([&](Cursor *cursor) {
    cursor->moveDown(lineCount, true);
  });
}

void TextEditor::moveLeft(double columnCount) {
  this->moveCursors([&](Cursor *cursor) {
    cursor->moveLeft(columnCount, true);
  });
}

void TextEditor::moveRight(double columnCount) {
  this->moveCursors([&](Cursor *cursor) {
    cursor->moveRight(columnCount, true);
  });
}

void TextEditor::moveToBeginningOfLine() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToBeginningOfLine(); });
}

void TextEditor::moveToBeginningOfScreenLine() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToBeginningOfScreenLine(); });
}

void TextEditor::moveToFirstCharacterOfLine() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToFirstCharacterOfLine(); });
}

void TextEditor::moveToEndOfLine() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToEndOfLine(); });
}

void TextEditor::moveToEndOfScreenLine() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToEndOfScreenLine(); });
}

void TextEditor::moveToBeginningOfWord() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToBeginningOfWord(); });
}

void TextEditor::moveToEndOfWord() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToEndOfWord(); });
}

void TextEditor::moveToTop() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToTop(); });
}

void TextEditor::moveToBottom() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToBottom(); });
}

void TextEditor::moveToBeginningOfNextWord() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToBeginningOfNextWord(); });
}

void TextEditor::moveToPreviousWordBoundary() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToPreviousWordBoundary(); });
}

void TextEditor::moveToNextWordBoundary() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToNextWordBoundary(); });
}

void TextEditor::moveToPreviousSubwordBoundary() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToPreviousSubwordBoundary(); });
}

void TextEditor::moveToNextSubwordBoundary() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToNextSubwordBoundary(); });
}

void TextEditor::moveToBeginningOfNextParagraph() {
  return this->moveCursors([](Cursor *cursor) { cursor->moveToBeginningOfNextParagraph(); });
}

void TextEditor::moveToBeginningOfPreviousParagraph() {
  return this->moveCursors([](Cursor *cursor) {
    cursor->moveToBeginningOfPreviousParagraph();
  });
}

Cursor *TextEditor::getLastCursor() {
  this->createLastSelectionIfNeeded();
  return this->cursors.back();
}

std::vector<Cursor *> TextEditor::getCursors() {
  this->createLastSelectionIfNeeded();
  return cursors;
}

std::vector<Cursor *> TextEditor::getCursorsOrderedByBufferPosition() {
  return sort(this->getCursors(), [](Cursor *a, Cursor *b) {
    return a->compare(b);
  });
}

Cursor *TextEditor::addCursor(DisplayMarker *marker) {
  Cursor *cursor = new Cursor(
    this,
    marker
    //showCursorOnSelection: this.showCursorOnSelection
  );
  this->cursors.push_back(cursor);
  this->cursorsByMarkerId[marker->id] = cursor;
  return cursor;
}

void TextEditor::moveCursors(std::function<void(Cursor *)> fn) {
  //return this->transact([&]() {
    for (Cursor *cursor: getCursors()) fn(cursor);
    this->mergeCursors();
  //});
}

void TextEditor::mergeCursors() {
  std::set<Point> positions;
  for (Cursor *cursor: getCursors()) {
    const Point position = cursor->getBufferPosition();
    if (positions.count(position)) {
      cursor->destroy();
    } else {
      positions.insert(position);
    }
  }
}

/*
Section: Selections
*/

std::u16string TextEditor::getSelectedText() {
  return this->getLastSelection()->getText();
}

Range TextEditor::getSelectedBufferRange() {
  return this->getLastSelection()->getBufferRange();
}

std::vector<Range> TextEditor::getSelectedBufferRanges() {
  std::vector<Range> ranges;
  for (Selection *selection : this->getSelections()) {
    ranges.push_back(selection->getBufferRange());
  }
  return ranges;
}

void TextEditor::setSelectedBufferRange(Range bufferRange /* , options */) {
  return this->setSelectedBufferRanges({bufferRange} /* , options */);
}

void TextEditor::setSelectedBufferRanges(const std::vector<Range> &bufferRanges /* , options = {} */) {
  //if (!bufferRanges.length)
  //  throw new Error('Passed an empty array to setSelectedBufferRanges');

  const auto selections = this->getSelections();
  for (size_t i = bufferRanges.size(); i < selections.size(); i++) {
    selections[i]->destroy();
  }

  this->mergeIntersectingSelections(/* options, */ [&]() {
    for (size_t i = 0; i < bufferRanges.size(); i++) {
      const Range bufferRange = bufferRanges[i];
      if (i < selections.size()) {
        selections[i]->setBufferRange(bufferRange /* , options */);
      } else {
        this->addSelectionForBufferRange(bufferRange /* , options */);
      }
    }
  });
}

Range TextEditor::getSelectedScreenRange() {
  return this->getLastSelection()->getScreenRange();
}

Selection *TextEditor::addSelectionForBufferRange(Range bufferRange) {
  this->selectionsMarkerLayer->markBufferRange(bufferRange /* , {
    invalidate: 'never',
    reversed: options.reversed != null ? options.reversed : false
  } */ );
  return this->getLastSelection();
}

Selection *TextEditor::addSelectionForScreenRange(Range screenRange /* , options = {} */ ) {
  return this->addSelectionForBufferRange(
    this->bufferRangeForScreenRange(screenRange) /* ,
    options */
  );
}

void TextEditor::selectToBufferPosition(Point position) {
  Selection *lastSelection = this->getLastSelection();
  lastSelection->selectToBufferPosition(position);
  return this->mergeIntersectingSelections(/*{
    reversed: lastSelection.isReversed()
  }*/);
}

void TextEditor::selectToScreenPosition(Point position, bool suppressSelectionMerge /* , options */) {
  Selection *lastSelection = this->getLastSelection();
  lastSelection->selectToScreenPosition(position /* , options */);
  if (!suppressSelectionMerge) {
    return this->mergeIntersectingSelections(/*{
      reversed: lastSelection.isReversed()
    }*/);
  }
}

void TextEditor::selectUp(double rowCount) {
  return this->expandSelectionsBackward([&](Selection *selection) {
    selection->selectUp(rowCount);
  });
}

void TextEditor::selectDown(double rowCount) {
  return this->expandSelectionsForward([&](Selection *selection) {
    selection->selectDown(rowCount);
  });
}

void TextEditor::selectLeft(double columnCount) {
  return this->expandSelectionsBackward([&](Selection *selection) {
    selection->selectLeft(columnCount);
  });
}

void TextEditor::selectRight(double columnCount) {
  return this->expandSelectionsForward([&](Selection *selection) {
    selection->selectRight(columnCount);
  });
}

void TextEditor::selectToTop() {
  return this->expandSelectionsBackward([&](Selection *selection) {
    selection->selectToTop();
  });
}

void TextEditor::selectToBottom() {
  return this->expandSelectionsForward([&](Selection *selection) {
    selection->selectToBottom();
  });
}

void TextEditor::selectAll() {
  return this->expandSelectionsForward([&](Selection *selection) {
    selection->selectAll();
  });
}

void TextEditor::selectToBeginningOfLine() {
  return this->expandSelectionsBackward([&](Selection *selection) {
    selection->selectToBeginningOfLine();
  });
}

void TextEditor::selectToFirstCharacterOfLine() {
  return this->expandSelectionsBackward([](Selection *selection) {
    selection->selectToFirstCharacterOfLine();
  });
}

void TextEditor::selectToEndOfLine() {
  return this->expandSelectionsForward([&](Selection *selection) {
    selection->selectToEndOfLine();
  });
}

void TextEditor::selectToBeginningOfWord() {
  return this->expandSelectionsBackward([](Selection *selection) {
    selection->selectToBeginningOfWord();
  });
}

void TextEditor::selectToEndOfWord() {
  return this->expandSelectionsForward([](Selection *selection) {
    selection->selectToEndOfWord();
  });
}

void TextEditor::selectToPreviousSubwordBoundary() {
  return this->expandSelectionsBackward([](Selection *selection) {
    selection->selectToPreviousSubwordBoundary();
  });
}

void TextEditor::selectToNextSubwordBoundary() {
  return this->expandSelectionsForward([](Selection *selection) {
    selection->selectToNextSubwordBoundary();
  });
}

void TextEditor::selectLinesContainingCursors() {
  return this->expandSelectionsForward([](Selection *selection) { selection->selectLine(); });
}

void TextEditor::selectWordsContainingCursors() {
  return this->expandSelectionsForward([](Selection *selection) { selection->selectWord(); });
}

void TextEditor::selectToPreviousWordBoundary() {
  return this->expandSelectionsBackward([](Selection *selection) {
    selection->selectToPreviousWordBoundary();
  });
}

void TextEditor::selectToNextWordBoundary() {
  return this->expandSelectionsForward([](Selection *selection) {
    selection->selectToNextWordBoundary();
  });
}

void TextEditor::selectToBeginningOfNextWord() {
  return this->expandSelectionsForward([](Selection *selection) {
    selection->selectToBeginningOfNextWord();
  });
}

void TextEditor::selectToBeginningOfNextParagraph() {
  return this->expandSelectionsForward([](Selection *selection) {
    selection->selectToBeginningOfNextParagraph();
  });
}

void TextEditor::selectToBeginningOfPreviousParagraph() {
  return this->expandSelectionsBackward([](Selection *selection) {
    selection->selectToBeginningOfPreviousParagraph();
  });
}

Selection *TextEditor::getLastSelection() {
  this->createLastSelectionIfNeeded();
  return this->selections.back();
}

Selection *TextEditor::getSelectionAtScreenPosition(Point position) {
  const auto markers = this->selectionsMarkerLayer->findMarkers({
    containsScreenPosition(position)
  });
  if (markers.size() > 0)
    return this->cursorsByMarkerId[markers[0]->id]->selection;
  return nullptr;
}

std::vector<Selection *> TextEditor::getSelections() {
  this->createLastSelectionIfNeeded();
  return this->selections;
}

std::vector<Selection *> TextEditor::getSelectionsOrderedByBufferPosition() {
  return sort(this->getSelections(), [](Selection *a, Selection *b) {
    return a->compare(b) < 0;
  });
}

void TextEditor::addSelectionBelow() {
  return this->expandSelectionsForward([](Selection *selection) {
    selection->addSelectionBelow();
  });
}

void TextEditor::addSelectionAbove() {
  return this->expandSelectionsBackward([](Selection *selection) {
    selection->addSelectionAbove();
  });
}

void TextEditor::expandSelectionsForward(std::function<void(Selection *)> fn) {
  this->mergeIntersectingSelections([&]() {
    for (Selection *selection : this->getSelections()) {
      fn(selection);
    }
  });
}

void TextEditor::expandSelectionsBackward(std::function<void(Selection *)> fn) {
  this->mergeIntersectingSelections(/* { reversed: true }, */ [&]() {
    for (Selection *selection : this->getSelections()) {
      fn(selection);
    }
  });
}

void TextEditor::finalizeSelections() {
  for (Selection *selection : this->getSelections()) {
    selection->finalize();
  }
}

void TextEditor::mergeIntersectingSelections( /* options, */ std::function<void()> fn) {
  return this->mergeSelections(
    fn,
    [](Selection *previousSelection, Selection *currentSelection) {
      const bool exclusive =
        !currentSelection->isEmpty() && !previousSelection->isEmpty();
      return previousSelection->intersectsWith(currentSelection, exclusive);
    }
  );
}

void TextEditor::mergeIntersectingSelections(/* options */) {
  return this->mergeIntersectingSelections([]() {});
}

void TextEditor::mergeSelectionsOnSameRows( /* options, */ std::function<void()> fn) {
  return this->mergeSelections(
    fn,
    [](Selection *previousSelection, Selection *currentSelection) {
      const Range screenRange = currentSelection->getScreenRange();
      return previousSelection->intersectsScreenRowRange(
        screenRange.start.row,
        screenRange.end.row
      );
    }
  );
}

void TextEditor::mergeSelectionsOnSameRows( /* options */) {
  return this->mergeSelectionsOnSameRows([]() {});
}

void TextEditor::avoidMergingSelections(std::function<void()> fn) {
  return this->mergeSelections(fn, [](Selection *, Selection *) {
    return false;
  });
}

void TextEditor::mergeSelections( /* options, */ std::function<void()> fn, std::function<bool(Selection *, Selection *)> mergePredicate) {
  //if (this.suppressSelectionMerging) return fn();

  //this.suppressSelectionMerging = true;
  fn();
  //this.suppressSelectionMerging = false;

  auto selections = this->getSelectionsOrderedByBufferPosition();
  Selection *lastSelection = selections.front();
  selections.erase(selections.begin());
  for (Selection *selection : selections) {
    if (mergePredicate(lastSelection, selection)) {
      lastSelection->merge(selection);
    } else {
      lastSelection = selection;
    }
  }
}

void TextEditor::addSelection(DisplayMarker *marker) {
  Cursor *cursor = this->addCursor(marker);
  Selection *selection = new Selection(
    this, marker, cursor
  );
  this->selections.push_back(selection);
  const Range selectionBufferRange = selection->getBufferRange();
  this->mergeIntersectingSelections( /* { preserveFolds: options.preserveFolds } */ );

  /*if (selection.destroyed) {
    for (selection of this.getSelections()) {
      if (selection.intersectsBufferRange(selectionBufferRange))
        return selection;
    }
  } else {
    this.emitter.emit('did-add-cursor', cursor);
    this.emitter.emit('did-add-selection', selection);
    return selection;
  }*/
}

void TextEditor::removeSelection(Selection *selection) {
  this->cursors.erase(std::find(this->cursors.begin(), this->cursors.end(), selection->cursor));
  this->selections.erase(std::find(this->selections.begin(), this->selections.end(), selection));
  this->cursorsByMarkerId.erase(this->cursorsByMarkerId.find(selection->cursor->getMarker()->id));
  delete selection->cursor;
  delete selection;
}

bool TextEditor::consolidateSelections() {
  const auto selections = this->getSelections();
  if (selections.size() > 1) {
    for (size_t i = 1; i < selections.size(); i++) {
      selections[i]->destroy();
    }
    //selections[0].autoscroll({ center: true });
    return true;
  } else {
    return false;
  }
}

void TextEditor::createLastSelectionIfNeeded() {
  if (this->selections.size() == 0) {
    this->addSelectionForBufferRange(Range(Point(0, 0), Point(0, 0)) /* , {
      autoscroll: false,
      preserveFolds: true
    } */ );
  }
}

/*
Section: Searching and Replacing
*/

void TextEditor::scan(const Regex &regex, /* options = {}, */ TextBuffer::ScanIterator iterator) {
  return this->buffer->scan(regex, /* options, */ iterator);
}

void TextEditor::scanInBufferRange(const Regex &regex, Range range, TextBuffer::ScanIterator iterator) {
  return this->buffer->scanInRange(regex, range, iterator);
}

void TextEditor::backwardsScanInBufferRange(const Regex &regex, Range range, TextBuffer::ScanIterator iterator) {
  return this->buffer->backwardsScanInRange(regex, range, iterator);
}

/*
Section: Tab Behavior
*/

bool TextEditor::getSoftTabs() {
  return this->softTabs;
}

double TextEditor::getTabLength() {
  return this->displayLayer->tabLength;
}

std::u16string TextEditor::getTabText() {
  return this->buildIndentString(1);
}

/*
Section: Soft Wrap Behavior
*/

/*
Section: Indentation
*/

double TextEditor::indentationForBufferRow(double bufferRow) {
  return this->indentLevelForLine(*this->lineTextForBufferRow(bufferRow));
}

Range TextEditor::setIndentationForBufferRow(
  double bufferRow,
  double newLevel,
  bool preserveLeadingWhitespace
) {
  double endColumn;
  if (preserveLeadingWhitespace) {
    endColumn = 0;
  } else {
    static const Regex regex(u"^\\s*", nullptr);
    endColumn = regex.match(this->lineTextForBufferRow(bufferRow)).end_offset;
  }
  std::u16string newIndentString = this->buildIndentString(newLevel);
  return this->buffer->setTextInRange(
    {{bufferRow, 0}, {bufferRow, endColumn}},
    std::move(newIndentString)
  );
}

void TextEditor::indentSelectedRows(/* options = {} */) {
  //if (!this->ensureWritable('indentSelectedRows', options)) return;
  return this->mutateSelectedText([](Selection *selection) {
    selection->indentSelectedRows(/* options */);
  });
}

void TextEditor::outdentSelectedRows(/* options = {} */) {
  //if (!this->ensureWritable('outdentSelectedRows', options)) return;
  return this->mutateSelectedText([](Selection *selection) {
    selection->outdentSelectedRows(/* options */);
  });
}

double TextEditor::indentLevelForLine(const std::u16string &line) {
  const double tabLength = this->getTabLength();
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

void TextEditor::indent(/* options = {} */) {
  //if (!this->ensureWritable('indent', options)) return;
  //if (options.autoIndent == null)
  //  options.autoIndent = this->shouldAutoIndent();
  this->mutateSelectedText([](Selection *selection) { selection->indent(/* options */); });
}

static std::u16string multiplyString(const char16_t *string, double n) {
  std::u16string finalString = u"";
  double i = 0;
  while (i < n) {
    finalString += string;
    i++;
  }
  return finalString;
}

std::u16string TextEditor::buildIndentString(double level, double column) {
  if (this->getSoftTabs()) {
    const double tabStopViolation = std::fmod(column, this->getTabLength());
    return multiplyString(
      u" ",
      std::floor(level * this->getTabLength()) - tabStopViolation
    );
  } else {
    const std::u16string excessWhitespace = multiplyString(
      u" ",
      std::round((level - std::floor(level)) * this->getTabLength())
    );
    return multiplyString(u"\t", std::floor(level)) + excessWhitespace;
  }
}

/*
Section: Grammars
*/

/*
Section: Managing Syntax Scopes
*/

/*
Section: Clipboard Operations
*/

/*
Section: Folds
*/

/*
Section: Gutters
*/

/*
Section: Scrolling the TextEditor
*/

/*
Section: Config
*/

bool TextEditor::shouldAutoIndent() {
  //return this.autoIndent;
  return true;
}

const char16_t *TextEditor::getNonWordCharacters(Point position) {
  //const languageMode = this->buffer->getLanguageMode();
  return (
    //(languageMode.getNonWordCharacters &&
    //  languageMode.getNonWordCharacters(position || Point(0, 0))) ||
    DEFAULT_NON_WORD_CHARACTERS
  );
}

/*
Section: Event Handlers
*/

/*
Section: TextEditor Rendering
*/

/*
Section: Utility
*/

/*
Section: Language Mode Delegated Methods
*/
