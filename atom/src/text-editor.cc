#include "text-editor.h"
#include "cursor.h"
#include "selection.h"
#include "decoration-manager.h"
#include <text-buffer.h>
#include <display-marker-layer.h>
#include <helpers.h>
#include <set>

constexpr const char16_t *DEFAULT_NON_WORD_CHARACTERS = u"/\\()\"':,.;<>~!@#$%^&*|+=[]{}`?-…";

TextEditor::TextEditor() {
  this->buffer = new TextBuffer();
  this->displayLayer = this->buffer->addDisplayLayer();
  //this->defaultMarkerLayer = this->displayLayer->addMarkerLayer();
  this->selectionsMarkerLayer = this->addMarkerLayer();
  this->decorationManager = new DecorationManager(this);
  this->decorateMarkerLayer(this->selectionsMarkerLayer, { Decoration::Type::cursor });

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
  delete buffer;
  delete decorationManager;
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

unsigned TextEditor::getLineCount() {
  return this->buffer->getLineCount();
}

double TextEditor::getScreenLineCount() {
  return this->displayLayer->getScreenLineCount();
}

double TextEditor::getApproximateScreenLineCount() {
  return this->displayLayer->getApproximateScreenLineCount();
}

unsigned TextEditor::getLastBufferRow() {
  return this->buffer->getLastRow();
}

double TextEditor::getLastScreenRow() {
  return this->getScreenLineCount() - 1;
}

optional<std::u16string> TextEditor::lineTextForBufferRow(uint32_t bufferRow) {
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

/*bufferRowsForScreenRows(startScreenRow, endScreenRow) {
  return this.displayLayer.bufferRowsForScreenRows(
    startScreenRow,
    endScreenRow + 1
  );
}*/

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

/*isBufferRowBlank(bufferRow) {
  return this.buffer.isRowBlank(bufferRow);
}*/

/*nextNonBlankBufferRow(bufferRow) {
  return this.buffer.nextNonBlankRow(bufferRow);
}*/

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
  //return this.mergeIntersectingSelections(() => {
    //return this.transact(groupingInterval, () => {
      for (Selection *selection : this->getSelectionsOrderedByBufferPosition()) fn(selection);
    //});
  //});
}

/*
Section: History
*/

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

bool TextEditor::hasMultipleCursors() const {
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

std::vector<Cursor *> TextEditor::getCursors() const {
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
  //this.cursorsByMarkerId.set(marker.id, cursor);
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

Selection *TextEditor::getLastSelection() {
  this->createLastSelectionIfNeeded();
  return this->selections.back();
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
  //return this->expandSelectionsForward([](Selection *selection) { selection->selectLine(); });
}

void TextEditor::selectWordsContainingCursors() {
  //return this->expandSelectionsForward([](Selection *selection) { selection->selectWord(); });
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

std::vector<Selection *> TextEditor::getSelections() {
  this->createLastSelectionIfNeeded();
  return this->selections;
}

std::vector<Selection *> TextEditor::getSelectionsOrderedByBufferPosition() {
  return sort(this->getSelections(), [](Selection *a, Selection *b) {
    return a->compare(b) < 0;
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

/*
Section: Soft Wrap Behavior
*/

/*
Section: Indentation
*/

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
