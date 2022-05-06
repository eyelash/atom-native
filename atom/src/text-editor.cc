#include "text-editor.h"
#include "cursor.h"
#include "selection.h"
#include <text-buffer.h>
#include <display-marker-layer.h>
#include <helpers.h>
#include <set>

TextEditor::TextEditor() {
  this->buffer = new TextBuffer();
  this->displayLayer = this->buffer->addDisplayLayer();
  //this->defaultMarkerLayer = this->displayLayer->addMarkerLayer();
  this->selectionsMarkerLayer = this->addMarkerLayer();

  this->subscribeToBuffer();
  this->subscribeToDisplayLayer();

  this->addCursorAtBufferPosition(Point(0, 0));
}

TextEditor::~TextEditor() {
  delete buffer;
}

void TextEditor::subscribeToBuffer() {

}

void TextEditor::subscribeToDisplayLayer() {
  using namespace std::placeholders;
  ;
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

/*getApproximateScreenLineCount() {
  return this.displayLayer.getApproximateScreenLineCount();
}*/

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

/*
Section: Decorations
*/

/*
Section: Markers
*/

DisplayMarkerLayer *TextEditor::addMarkerLayer() {
  return this->displayLayer->addMarkerLayer();
}

/*
Section: Cursors
*/

void TextEditor::setCursorScreenPosition(Point position) {
  this->moveCursors([position](Cursor *cursor) {
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

std::vector<Cursor *> TextEditor::getCursors() const {
  return cursors;
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
      //cursor.destroy();
    } else {
      positions.insert(position);
    }
  }
}

/*
Section: Selections
*/

Selection *TextEditor::addSelectionForBufferRange(Range bufferRange) {
  this->selectionsMarkerLayer->markBufferRange(bufferRange /* , {
    invalidate: 'never',
    reversed: options.reversed != null ? options.reversed : false
  } */ );
  return this->getLastSelection();
}

Selection *TextEditor::getLastSelection() {
  this->createLastSelectionIfNeeded();
  return this->selections.back();
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

void TextEditor::addSelection(DisplayMarker *marker) {
  Cursor *cursor = this->addCursor(marker);
  Selection *selection = new Selection(
    this, marker, cursor
  );
  this->selections.push_back(selection);
  /*const selectionBufferRange = selection.getBufferRange();
  this.mergeIntersectingSelections({ preserveFolds: options.preserveFolds });

  if (selection.destroyed) {
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
