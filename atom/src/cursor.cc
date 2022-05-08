#include "cursor.h"
#include "text-editor.h"
#include <display-marker.h>
#include <cmath>

Cursor::Cursor(TextEditor *editor, DisplayMarker *marker) {
  this->editor = editor;
  this->marker = marker;
}

Cursor::~Cursor() {}

void Cursor::destroy() {
  this->marker->destroy();
}

void Cursor::setScreenPosition(Point screenPosition, optional<bool> options_autoscroll) {
  this->changePosition(options_autoscroll, [&]() {
    this->marker->setHeadScreenPosition(screenPosition);
  });
}

Point Cursor::getScreenPosition() {
  return this->marker->getHeadScreenPosition();
}

void Cursor::setBufferPosition(Point bufferPosition, optional<bool> options_autoscroll) {
  this->changePosition(options_autoscroll, [&]() {
    this->marker->setHeadBufferPosition(bufferPosition);
  });
}

Point Cursor::getBufferPosition() {
  return this->marker->getHeadBufferPosition();
}

double Cursor::getScreenRow() {
  return this->getScreenPosition().row;
}

double Cursor::getScreenColumn() {
  return this->getScreenPosition().column;
}

double Cursor::getBufferRow() {
  return this->getBufferPosition().row;
}

double Cursor::getBufferColumn() {
  return this->getBufferPosition().column;
}

std::u16string Cursor::getCurrentBufferLine() {
  return *this->editor->lineTextForBufferRow(this->getBufferRow());
}

bool Cursor::isAtBeginningOfLine() {
  return this->getBufferPosition().column == 0;
}

bool Cursor::isAtEndOfLine() {
  return this->getBufferPosition().isEqual(
    this->getCurrentLineBufferRange().end
  );
}

/*
Section: Cursor Position Details
*/

DisplayMarker *Cursor::getMarker() {
  return this->marker;
}

/*isSurroundedByWhitespace() {
  const { row, column } = this.getBufferPosition();
  const range = [[row, column - 1], [row, column + 1]];
  return /^\s+$/.test(this.editor.getTextInBufferRange(range));
}*/

/*isBetweenWordAndNonWord() {
  if (this.isAtBeginningOfLine() || this.isAtEndOfLine()) return false;

  const { row, column } = this.getBufferPosition();
  const range = [[row, column - 1], [row, column + 1]];
  const text = this.editor.getTextInBufferRange(range);
  if (/\s/.test(text[0]) || /\s/.test(text[1])) return false;

  const nonWordCharacters = this.getNonWordCharacters();
  return (
    nonWordCharacters.includes(text[0]) !==
    nonWordCharacters.includes(text[1])
  );
}*/

/*isInsideWord(options) {
  const { row, column } = this.getBufferPosition();
  const range = [[row, column], [row, Infinity]];
  const text = this.editor.getTextInBufferRange(range);
  return (
    text.search((options && options.wordRegex) || this.wordRegExp()) === 0
  );
}*/

/*getIndentLevel() {
  if (this.editor.getSoftTabs()) {
    return this.getBufferColumn() / this.editor.getTabLength();
  } else {
    return this.getBufferColumn();
  }
}*/

/*getScopeDescriptor() {
  return this.editor.scopeDescriptorForBufferPosition(
    this.getBufferPosition()
  );
}*/

/*getSyntaxTreeScopeDescriptor() {
  return this.editor.syntaxTreeScopeDescriptorForBufferPosition(
    this.getBufferPosition()
  );
}*/

/*hasPrecedingCharactersOnLine() {
  const bufferPosition = this.getBufferPosition();
  const line = this.editor.lineTextForBufferRow(bufferPosition.row);
  const firstCharacterColumn = line.search(/\S/);

  if (firstCharacterColumn === -1) {
    return false;
  } else {
    return bufferPosition.column > firstCharacterColumn;
  }
}*/

bool Cursor::isLastCursor() {
  return this == this->editor->getLastCursor();
}

/*
Section: Moving the Cursor
*/

void Cursor::moveUp(double rowCount, bool moveToEndOfSelection) {
  Point position;
  const Range range = this->marker->getScreenRange();
  if (moveToEndOfSelection && !range.isEmpty()) {
    position = range.start;
  } else {
    position = this->getScreenPosition();
  }

  if (this->goalColumn) position.column = *this->goalColumn;
  this->setScreenPosition(
    Point(position.row - rowCount, position.column) /* ,
    { skipSoftWrapIndentation: true } */
  );
  this->goalColumn = position.column;
}

void Cursor::moveDown(double rowCount, bool moveToEndOfSelection) {
  Point position;
  const Range range = this->marker->getScreenRange();
  if (moveToEndOfSelection && !range.isEmpty()) {
    position = range.end;
  } else {
    position = this->getScreenPosition();
  }

  if (this->goalColumn) position.column = *this->goalColumn;
  this->setScreenPosition(
    Point(position.row + rowCount, position.column) /* ,
    { skipSoftWrapIndentation: true } */
  );
  this->goalColumn = position.column;
}

void Cursor::moveLeft(double columnCount, bool moveToEndOfSelection) {
  const Range range = this->marker->getScreenRange();
  if (moveToEndOfSelection && !range.isEmpty()) {
    this->setScreenPosition(range.start);
  } else {
    const Point screenPosition = this->getScreenPosition();
    double row = screenPosition.row, column = screenPosition.column;

    while (columnCount > column && row > 0) {
      columnCount -= column;
      column = this->editor->lineLengthForScreenRow(--row);
      columnCount--; // subtract 1 for the row move
    }

    column = column - columnCount;
    this->setScreenPosition(Point(row, column) /* , { clipDirection: 'backward' } */ );
  }
}

void Cursor::moveRight(double columnCount, bool moveToEndOfSelection) {
  const Range range = this->marker->getScreenRange();
  if (moveToEndOfSelection && !range.isEmpty()) {
    this->setScreenPosition(range.end);
  } else {
    const Point screenPosition = this->getScreenPosition();
    double row = screenPosition.row, column = screenPosition.column;
    const double maxLines = this->editor->getScreenLineCount();
    double rowLength = this->editor->lineLengthForScreenRow(row);
    double columnsRemainingInLine = rowLength - column;

    while (columnCount > columnsRemainingInLine && row < maxLines - 1) {
      columnCount -= columnsRemainingInLine;
      columnCount--; // subtract 1 for the row move

      column = 0;
      rowLength = this->editor->lineLengthForScreenRow(++row);
      columnsRemainingInLine = rowLength;
    }

    column = column + columnCount;
    this->setScreenPosition(Point(row, column) /* , { clipDirection: 'forward' } */ );
  }
}

void Cursor::moveToTop() {
  this->setBufferPosition(Point(0, 0));
}

void Cursor::moveToBottom() {
  const optional<double> column = this->goalColumn;
  this->setBufferPosition(this->editor->getEofBufferPosition());
  this->goalColumn = column;
}

void Cursor::moveToBeginningOfScreenLine() {
  this->setScreenPosition(Point(this->getScreenRow(), 0));
}

void Cursor::moveToBeginningOfLine() {
  this->setBufferPosition(Point(this->getBufferRow(), 0));
}

void Cursor::moveToEndOfScreenLine() {
  this->setScreenPosition(Point(this->getScreenRow(), INFINITY));
}

void Cursor::moveToEndOfLine() {
  this->setBufferPosition(Point(this->getBufferRow(), INFINITY));
}

Range Cursor::getCurrentLineBufferRange(bool includeNewline) {
  return this->editor->bufferRangeForBufferRow(this->getBufferRow(), includeNewline);
}

/*
Section: Visibility
*/

/*
Section: Comparing to another cursor
*/

int Cursor::compare(Cursor *otherCursor) {
  return this->getBufferPosition().compare(otherCursor->getBufferPosition());
}

/*
Section: Utilities
*/

void Cursor::clearSelection(bool autoscroll) {
  //if (this.selection) this.selection.clear(options);
}

/*
Section: Private
*/

void Cursor::changePosition(optional<bool> options_autoscroll, std::function<void()> fn) {
  this->clearSelection(false);
  fn();
  this->goalColumn = optional<double>();
  //const bool autoscroll =
  //  options_autoscroll
  //    ? *options_autoscroll
  //    : this->isLastCursor();
  //if (autoscroll) this.autoscroll();
}
