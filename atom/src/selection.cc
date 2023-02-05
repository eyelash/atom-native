#include "selection.h"
#include "cursor.h"
#include "text-editor.h"
#include <display-marker.h>
#include <display-marker-layer.h>
#include <text-buffer.h>

Selection::Selection(TextEditor *editor, DisplayMarker *marker, Cursor *cursor) {
  this->editor = editor;
  this->marker = marker;
  this->wordwise = false;
  this->linewise = false;
  this->retainSelection = false;
  this->cursor = cursor;
  this->cursor->selection = this;
  /* this->decoration = */ this->editor->decorateMarker(this->marker, {
    Decoration::Type::highlight,
    "selection"
  });
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

void Selection::setScreenRange(Range screenRange, optional<bool> reversed) {
  return this->setBufferRange(
    this->editor->bufferRangeForScreenRange(screenRange),
    reversed
  );
}

Range Selection::getBufferRange() {
  return this->marker->getBufferRange();
}

void Selection::setBufferRange(Range bufferRange, optional<bool> reversed) {
  //if (options.reversed == null) options.reversed = this.isReversed();
  /*if (!options.preserveFolds)
    this.editor.destroyFoldsContainingBufferPositions(
      [bufferRange.start, bufferRange.end],
      true
    );*/
  this->modifySelection([&]() {
    //const needsFlash = options.flash;
    //options.flash = null;
    this->marker->setBufferRange(bufferRange, reversed);
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
  this->goalScreenRange = optional<Range>();
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

    if (this->linewise) {
      this->expandOverLine(/* options */);
    } else if (this->wordwise) {
      this->expandOverWord(/* options */);
    }
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

void Selection::selectWord(/* options = {} */) {
  const Regex *wordRegex = nullptr;
  if (this->cursor->isSurroundedByWhitespace()) {
    static const Regex whitespaceWordRegex(u"[\t ]*", nullptr);
    wordRegex = &whitespaceWordRegex;
  }
  bool includeNonWordCharacters = true;
  if (this->cursor->isBetweenWordAndNonWord()) {
    includeNonWordCharacters = false;
  }

  this->setBufferRange(
    this->cursor->getCurrentWordBufferRange(wordRegex, includeNonWordCharacters) /* ,
    options */
  );
  this->wordwise = true;
  //this.initialScreenRange = this.getScreenRange();
}

void Selection::expandOverWord(/* options */) {
  this->setBufferRange(
    this->getBufferRange().union_(this->cursor->getCurrentWordBufferRange()) /* ,
    { autoscroll: false } */
  );
  //const autoscroll =
  //  options && options.autoscroll != null
  //    ? options.autoscroll
  //    : this->isLastSelection();
  //if (autoscroll) this->cursor.autoscroll();
}

void Selection::selectLine(optional<double> row /* , options */) {
  if (row) {
    this->setBufferRange(
      this->editor->bufferRangeForBufferRow(*row, true) /* ,
      options */
    );
  } else {
    const Range startRange = this->editor->bufferRangeForBufferRow(
      this->marker->getStartBufferPosition().row
    );
    const Range endRange = this->editor->bufferRangeForBufferRow(
      this->marker->getEndBufferPosition().row,
      true
    );
    this->setBufferRange(startRange.union_(endRange) /* , options */ );
  }

  this->linewise = true;
  this->wordwise = false;
  //this->initialScreenRange = this->getScreenRange();
}

void Selection::expandOverLine(/* options */) {
  const Range range = this->getBufferRange().union_(
    this->cursor->getCurrentLineBufferRange(true)
  );
  this->setBufferRange(range /* , { autoscroll: false } */);
  //const autoscroll =
  //  options && options.autoscroll != null
  //    ? options.autoscroll
  //    : this->isLastSelection();
  //if (autoscroll) this->cursor.autoscroll();
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

void Selection::deleteToPreviousWordBoundary(/* options = {} */) {
  //if (!this->ensureWritable('deleteToPreviousWordBoundary', options)) return;
  if (this->isEmpty()) this->selectToPreviousWordBoundary();
  this->deleteSelectedText(/* options */);
}

void Selection::deleteToNextWordBoundary(/* options = {} */) {
  //if (!this->ensureWritable('deleteToNextWordBoundary', options)) return;
  if (this->isEmpty()) this->selectToNextWordBoundary();
  this->deleteSelectedText(/* options */);
}

void Selection::deleteToBeginningOfWord(/* options = {} */) {
  //if (!this->ensureWritable('deleteToBeginningOfWord', options)) return;
  if (this->isEmpty()) this->selectToBeginningOfWord();
  this->deleteSelectedText(/* options */);
}

void Selection::deleteToBeginningOfLine(/* options = {} */) {
  //if (!this->ensureWritable('deleteToBeginningOfLine', options)) return;
  if (this->isEmpty() && this->cursor->isAtBeginningOfLine()) {
    this->selectLeft();
  } else {
    this->selectToBeginningOfLine();
  }
  this->deleteSelectedText(/* options */);
}

void Selection::delete_() {
  //if (!this->ensureWritable('delete', options)) return;
  if (this->isEmpty()) this->selectRight();
  this->deleteSelectedText();
}

void Selection::deleteToEndOfLine(/* options = {} */) {
  //if (!this->ensureWritable('deleteToEndOfLine', options)) return;
  if (this->isEmpty()) {
    if (this->cursor->isAtEndOfLine()) {
      this->delete_(/* options */);
      return;
    }
    this->selectToEndOfLine();
  }
  this->deleteSelectedText(/* options */);
}

void Selection::deleteToEndOfWord(/* options = {} */) {
  //if (!this->ensureWritable('deleteToEndOfWord', options)) return;
  if (this->isEmpty()) this->selectToEndOfWord();
  this->deleteSelectedText(/* options */);
}

void Selection::deleteToBeginningOfSubword(/* options = {} */) {
  //if (!this->ensureWritable('deleteToBeginningOfSubword', options)) return;
  if (this->isEmpty()) this->selectToPreviousSubwordBoundary();
  this->deleteSelectedText(/* options */);
}

void Selection::deleteToEndOfSubword(/* options = {} */) {
  //if (!this->ensureWritable('deleteToEndOfSubword', options)) return;
  if (this->isEmpty()) this->selectToNextSubwordBoundary();
  this->deleteSelectedText(/* options */);
}

void Selection::deleteSelectedText() {
  //if (!this->ensureWritable('deleteSelectedText', options)) return;
  const Range bufferRange = this->getBufferRange();
  if (!bufferRange.isEmpty()) this->editor->getBuffer()->delete_(bufferRange);
  if (this->cursor) this->cursor->setBufferPosition(bufferRange.start);
}

void Selection::deleteLine(/* options = {} */) {
  //if (!this->ensureWritable('deleteLine', options)) return;
  const Range range = this->getBufferRange();
  if (range.isEmpty()) {
    const double start = this->cursor->getScreenRow();
    const auto range = this->editor->bufferRowsForScreenRows(start, start + 1);
    if (range[1] > range[0]) {
      this->editor->getBuffer()->deleteRows(range[0], range[1] - 1);
    } else {
      this->editor->getBuffer()->deleteRow(range[0]);
    }
  } else {
    const double start = range.start.row;
    double end = range.end.row;
    if (end != this->editor->getBuffer()->getLastRow() && range.end.column == 0)
      end--;
    this->editor->getBuffer()->deleteRows(start, end);
  }
  this->cursor->setBufferPosition({
    this->cursor->getBufferRow(),
    range.start.column
  });
}

static std::u16string toString(double n) {
  if (n < 10) {
    std::u16string result;
    result.push_back(u'0' + n);
    return result;
  } else {
    std::u16string result = toString(n / 10.0);
    result.push_back(u'0' + std::fmod(n, 10.0));
    return result;
  }
}

void Selection::outdentSelectedRows(/* options = {} */) {
  //if (!this->ensureWritable('outdentSelectedRows', options)) return;
  const auto bufferRowRange = this->getBufferRowRange();
  const double start = bufferRowRange.first, end = bufferRowRange.second;
  TextBuffer *buffer = this->editor->getBuffer();
  const Regex leadingTabRegex = Regex(
    u"^( {1," + toString(this->editor->getTabLength()) + u"}|\\t)",
    nullptr
  );
  for (double row = start; row <= end; row++) {
    const auto match = leadingTabRegex.match(buffer->lineForRow(row));
    if (match && match.end_offset > match.start_offset) {
      buffer->delete_({{row, static_cast<double>(match.start_offset)}, {row, static_cast<double>(match.end_offset)}});
    }
  }
}

void Selection::indent(bool autoIndent) {
  //if (!this->ensureWritable('indent', { bypassReadOnly })) return;
  const double row = this->cursor->getBufferPosition().row;

  if (this->isEmpty()) {
    this->cursor->skipLeadingWhitespace();
    /*const desiredIndent = this->editor->suggestedIndentForBufferRow(row);
    let delta = desiredIndent - this->cursor->getIndentLevel();

    if (autoIndent && delta > 0) {
      if (!this->editor->getSoftTabs()) delta = Math.max(delta, 1);
      this->insertText(this->editor->buildIndentString(delta), {
        bypassReadOnly
      });
    } else */ {
      this->insertText(
        this->editor->buildIndentString(1, this->cursor->getBufferColumn())
      );
    }
  } else {
    this->indentSelectedRows();
  }
}

void Selection::indentSelectedRows(/* options = {} */) {
  //if (!this->ensureWritable('indentSelectedRows', options)) return;
  const auto bufferRowRange = this->getBufferRowRange();
  const double start = bufferRowRange.first, end = bufferRowRange.second;
  for (double row = start; row <= end; row++) {
    if (this->editor->getBuffer()->lineLengthForRow(row) != 0) {
      this->editor->getBuffer()->insert({row, 0}, this->editor->getTabText());
    }
  }
}

/*
Section: Managing multiple selections
*/

void Selection::addSelectionBelow() {
  Range range = this->getGoalScreenRange();
  const double nextRow = range.end.row + 1;

  for (
    double row = nextRow, end = this->editor->getLastScreenRow();
    row <= end;
    row++
  ) {
    range.start.row = row;
    range.end.row = row;
    const Range clippedRange = this->editor->clipScreenRange(range /* , {
      skipSoftWrapIndentation: true
    } */);

    if (range.isEmpty()) {
      if (range.end.column > 0 && clippedRange.end.column == 0) continue;
    } else {
      if (clippedRange.isEmpty()) continue;
    }

    const auto containingSelections = this->editor->selectionsMarkerLayer->findMarkers({
      containsScreenRange(clippedRange)
    });
    if (containingSelections.size() == 0) {
      Selection *selection = this->editor->addSelectionForScreenRange(clippedRange);
      selection->setGoalScreenRange(range);
    }

    break;
  }
}

void Selection::addSelectionAbove() {
  Range range = this->getGoalScreenRange();
  const double previousRow = range.end.row - 1;

  for (double row = previousRow; row >= 0; row--) {
    range.start.row = row;
    range.end.row = row;
    const Range clippedRange = this->editor->clipScreenRange(range /* , {
      skipSoftWrapIndentation: true
    } */);

    if (range.isEmpty()) {
      if (range.end.column > 0 && clippedRange.end.column == 0) continue;
    } else {
      if (clippedRange.isEmpty()) continue;
    }

    const auto containingSelections = this->editor->selectionsMarkerLayer->findMarkers({
      containsScreenRange(clippedRange)
    });
    if (containingSelections.size() == 0) {
      Selection *selection = this->editor->addSelectionForScreenRange(clippedRange);
      selection->setGoalScreenRange(range);
    }

    break;
  }
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
Section: Comparing to other selections
*/

int Selection::compare(Selection *otherSelection) {
  return this->marker->compare(otherSelection->marker);
}

/*
Section: Private Utilities
*/

void Selection::setGoalScreenRange(Range range) {
  this->goalScreenRange = range;
}

Range Selection::getGoalScreenRange() {
  return this->goalScreenRange ? *this->goalScreenRange : this->getScreenRange();
}

void Selection::markerDidDestroy() {
  this->editor->removeSelection(this);
}

void Selection::finalize() {
  /*if (
    !this->initialScreenRange ||
    !this->initialScreenRange.isEqual(this->getScreenRange())
  ) {
    this->initialScreenRange = null;
  }*/
  if (this->isEmpty()) {
    this->wordwise = false;
    this->linewise = false;
  }
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
