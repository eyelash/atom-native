#include "selection.h"
#include "cursor.h"
#include "text-editor.h"
#include <display-marker.h>
#include <text-buffer.h>

Selection::Selection(TextEditor *editor, DisplayMarker *marker, Cursor *cursor) {
  this->editor = editor;
  this->marker = marker;
  this->retainSelection = false;
  this->cursor = cursor;
  this->cursor->selection = this;
  this->marker->onDidDestroy([this]() { this->markerDidDestroy(); });
}

Selection::~Selection() {}

void Selection::destroy() {
  this->marker->destroy();
}

bool Selection::isLastSelection() {
  return this == this->editor->getLastSelection();
}

/*
Section: Event Subscription
*/

/*
Section: Managing the selection range
*/

Range Selection::getScreenRange() {
  return this->marker->getScreenRange();
}

Range Selection::getBufferRange() {
  return this->marker->getBufferRange();
}

void Selection::setBufferRange(Range bufferRange) {
  //if (options.reversed == null) options.reversed = this.isReversed();
  /*if (!options.preserveFolds)
    this.editor.destroyFoldsContainingBufferPositions(
      [bufferRange.start, bufferRange.end],
      true
    );*/
  this->modifySelection([&]() {
    //const needsFlash = options.flash;
    //options.flash = null;
    this->marker->setBufferRange(bufferRange);
    /*const autoscroll =
      options.autoscroll != null
        ? options.autoscroll
        : this.isLastSelection();
    if (autoscroll) this.autoscroll();*/
    /*if (needsFlash)
      this.decoration.flash('flash', this.editor.selectionFlashDuration);*/
  });
}

std::pair<double, double> Selection::getBufferRowRange() {
  const Range range = this->getBufferRange();
  const double start = range.start.row;
  double end = range.end.row;
  if (range.end.column == 0) end = std::max(start, end - 1);
  return std::make_pair(start, end);
}

Point Selection::getTailScreenPosition() {
  return this->marker->getTailScreenPosition();
}

Point Selection::getTailBufferPosition() {
  return this->marker->getTailBufferPosition();
}

Point Selection::getHeadScreenPosition() {
  return this->marker->getHeadScreenPosition();
}

Point Selection::getHeadBufferPosition() {
  return this->marker->getHeadBufferPosition();
}

/*
Section: Info about the selection
*/

bool Selection::isEmpty() {
  return this->getBufferRange().isEmpty();
}

bool Selection::isReversed() {
  return this->marker->isReversed();
}

bool Selection::isSingleScreenLine() {
  return this->getScreenRange().isSingleLine();
}

std::u16string Selection::getText() {
  return this->editor->getBuffer()->getTextInRange(this->getBufferRange());
}

bool Selection::intersectsBufferRange(Range bufferRange) {
  return this->getBufferRange().intersectsWith(bufferRange);
}

bool Selection::intersectsScreenRowRange(double startRow, double endRow) {
  return this->getScreenRange().intersectsRowRange(startRow, endRow);
}

bool Selection::intersectsScreenRow(double screenRow) {
  return this->getScreenRange().intersectsRow(screenRow);
}

bool Selection::intersectsWith(Selection *otherSelection, bool exclusive) {
  return this->getBufferRange().intersectsWith(
    otherSelection->getBufferRange(),
    exclusive
  );
}

/*
Section: Modifying the selected range
*/

void Selection::clear() {
  //this.goalScreenRange = null;
  if (!this->retainSelection) this->marker->clearTail();
  /*const autoscroll =
    options && options.autoscroll != null
      ? options.autoscroll
      : this.isLastSelection();
  if (autoscroll) this.autoscroll();*/
  //this.finalize();
}

void Selection::selectToScreenPosition(Point position) {
  this->modifySelection([&]() {
    /* if (this.initialScreenRange) {
      if (position.isLessThan(this.initialScreenRange.start)) {
        this.marker.setScreenRange([position, this.initialScreenRange.end], {
          reversed: true
        });
      } else {
        this.marker.setScreenRange(
          [this.initialScreenRange.start, position],
          { reversed: false }
        );
      }
    } else */ {
      this->cursor->setScreenPosition(position);
    }

    /*if (this.linewise) {
      this.expandOverLine(options);
    } else if (this.wordwise) {
      this.expandOverWord(options);
    }*/
  });
}

void Selection::selectToBufferPosition(Point position) {
  this->modifySelection([&]() { this->cursor->setBufferPosition(position); });
}

void Selection::selectRight(double columnCount) {
  this->modifySelection([&]() { this->cursor->moveRight(columnCount); });
}

void Selection::selectLeft(double columnCount) {
  this->modifySelection([&]() { this->cursor->moveLeft(columnCount); });
}

void Selection::selectUp(double rowCount) {
  this->modifySelection([&]() { this->cursor->moveUp(rowCount); });
}

void Selection::selectDown(double rowCount) {
  this->modifySelection([&]() { this->cursor->moveDown(rowCount); });
}

void Selection::selectToTop() {
  this->modifySelection([&]() { this->cursor->moveToTop(); });
}

void Selection::selectToBottom() {
  this->modifySelection([&]() { this->cursor->moveToBottom(); });
}

void Selection::selectAll() {
  this->setBufferRange(this->editor->getBuffer()->getRange() /* , { autoscroll: false } */);
}

void Selection::selectToBeginningOfLine() {
  this->modifySelection([&]() { this->cursor->moveToBeginningOfLine(); });
}

void Selection::selectToFirstCharacterOfLine() {
  this->modifySelection([&]() { this->cursor->moveToFirstCharacterOfLine(); });
}

void Selection::selectToEndOfLine() {
  this->modifySelection([&]() { this->cursor->moveToEndOfScreenLine(); });
}

void Selection::selectToEndOfBufferLine() {
  this->modifySelection([&]() { this->cursor->moveToEndOfLine(); });
}

void Selection::selectToBeginningOfWord() {
  this->modifySelection([&]() { this->cursor->moveToBeginningOfWord(); });
}

void Selection::selectToEndOfWord() {
  this->modifySelection([&]() { this->cursor->moveToEndOfWord(); });
}

void Selection::selectToBeginningOfNextWord() {
  this->modifySelection([&]() { this->cursor->moveToBeginningOfNextWord(); });
}

void Selection::selectToPreviousWordBoundary() {
  this->modifySelection([&]() { this->cursor->moveToPreviousWordBoundary(); });
}

void Selection::selectToNextWordBoundary() {
  this->modifySelection([&]() { this->cursor->moveToNextWordBoundary(); });
}

void Selection::selectToPreviousSubwordBoundary() {
  this->modifySelection([&]() { this->cursor->moveToPreviousSubwordBoundary(); });
}

void Selection::selectToNextSubwordBoundary() {
  this->modifySelection([&]() { this->cursor->moveToNextSubwordBoundary(); });
}

void Selection::selectToBeginningOfNextParagraph() {
  this->modifySelection([&]() { this->cursor->moveToBeginningOfNextParagraph(); });
}

void Selection::selectToBeginningOfPreviousParagraph() {
  this->modifySelection([&]() {
    this->cursor->moveToBeginningOfPreviousParagraph();
  });
}

/*
Section: Modifying the selected text
*/

void Selection::insertText(const std::u16string &text) {
  const Range oldBufferRange = this->getBufferRange();
  const bool wasReversed = this->isReversed();
  this->clear();

  const Range newBufferRange = this->editor->getBuffer()->setTextInRange(
    oldBufferRange,
    text
  );
}

void Selection::backspace() {
  //if (!this->ensureWritable('backspace', options)) return;
  if (this->isEmpty()) this->selectLeft();
  this->deleteSelectedText();
}

void Selection::delete_() {
  //if (!this->ensureWritable('delete', options)) return;
  if (this->isEmpty()) this->selectRight();
  this->deleteSelectedText();
}

void Selection::deleteSelectedText() {
  //if (!this->ensureWritable('deleteSelectedText', options)) return;
  const Range bufferRange = this->getBufferRange();
  if (!bufferRange.isEmpty()) this->editor->getBuffer()->delete_(bufferRange);
  if (this->cursor) this->cursor->setBufferPosition(bufferRange.start);
}

void Selection::merge(Selection *otherSelection /*, options = {} */) {
  //const Range myGoalScreenRange = this->getGoalScreenRange();
  //const Range otherGoalScreenRange = otherSelection->getGoalScreenRange();

  /*if (myGoalScreenRange && otherGoalScreenRange) {
    options.goalScreenRange = myGoalScreenRange.union_(otherGoalScreenRange);
  } else {
    options.goalScreenRange = myGoalScreenRange || otherGoalScreenRange;
  }*/

  const Range bufferRange = this->getBufferRange().union_(
    otherSelection->getBufferRange()
  );
  this->setBufferRange(
    bufferRange /* ,
    Object.assign({ autoscroll: false }, options) */
  );
  otherSelection->destroy();
}

/*
Section: Managing multiple selections
*/

/*
Section: Comparing to other selections
*/

int Selection::compare(Selection *otherSelection) {
  return this->marker->compare(otherSelection->marker);
}

/*
Section: Private Utilities
*/

void Selection::markerDidDestroy() {
  this->editor->removeSelection(this);
}

void Selection::modifySelection(std::function<void()> fn) {
  this->retainSelection = true;
  this->plantTail();
  fn();
  this->retainSelection = false;
}

void Selection::plantTail() {
  this->marker->plantTail();
}
