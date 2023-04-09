#include "cursor.h"
#include "selection.h"
#include "text-editor.h"
#include <display-marker.h>
#include <text-buffer.h>
#include <helpers.h>
#include <cmath>

static const Regex EmptyLineRegExp = Regex(u"(\r\n[\t ]*\r\n)|(\n[\t ]*\n)");

Cursor::Cursor(TextEditor *editor, DisplayMarker *marker) {
  this->editor = editor;
  this->marker = marker;
  this->selection = nullptr;
}

Cursor::~Cursor() {}

void Cursor::destroy() {
  this->marker->destroy();
}

void Cursor::setScreenPosition(Point screenPosition, optional<bool> options_autoscroll, DisplayLayer::ClipDirection clipDirection) {
  this->changePosition(options_autoscroll, [&]() {
    this->marker->setHeadScreenPosition(screenPosition, clipDirection);
  });
}

void Cursor::setScreenPosition(Point screenPosition, DisplayLayer::ClipDirection clipDirection) {
  this->setScreenPosition(screenPosition, optional<bool>(), clipDirection);
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

bool Cursor::isSurroundedByWhitespace() {
  const Point bufferPosition = this->getBufferPosition();
  const double row = bufferPosition.row, column = bufferPosition.column;
  const Range range = {{row, column - 1}, {row, column + 1}};
  static const Regex regex(u"^\\s+$", nullptr);
  return regex.match(this->editor->getTextInBufferRange(range));
}

bool Cursor::isBetweenWordAndNonWord() {
  if (this->isAtBeginningOfLine() || this->isAtEndOfLine()) return false;

  const Point bufferPosition = this->getBufferPosition();
  const double row = bufferPosition.row, column = bufferPosition.column;
  const Range range = {{row, column - 1}, {row, column + 1}};
  const std::u16string text = this->editor->getTextInBufferRange(range);
  static const Regex regex(u"\\s", nullptr);
  if (regex.match(text[0]) || regex.match(text[1])) return false;

  const char16_t *nonWordCharacters = this->getNonWordCharacters();
  return (
    includes(nonWordCharacters, text[0]) !=
    includes(nonWordCharacters, text[1])
  );
}

bool Cursor::isInsideWord(/* options */) {
  const Point bufferPosition = this->getBufferPosition();
  const double row = bufferPosition.row, column = bufferPosition.column;
  const Range range = {{row, column}, {row, INFINITY}};
  const std::u16string text = this->editor->getTextInBufferRange(range);
  const Regex wordRegex = /* (options && options.wordRegex) || */ this->wordRegExp();
  const auto match = wordRegex.match(text);
  return match && match.start_offset == 0;
}

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
    this->setScreenPosition({row, column}, DisplayLayer::ClipDirection::backward);
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
    this->setScreenPosition({row, column}, DisplayLayer::ClipDirection::forward);
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

void Cursor::moveToFirstCharacterOfLine() {
  double targetBufferColumn;
  const double screenRow = this->getScreenRow();
  const Point screenLineStart = this->editor->clipScreenPosition({screenRow, 0} /* , {
    skipSoftWrapIndentation: true
  } */);
  const Point screenLineEnd = {screenRow, INFINITY};
  const Range screenLineBufferRange = this->editor->bufferRangeForScreenRange({
    screenLineStart,
    screenLineEnd
  });

  optional<double> firstCharacterColumn;
  this->editor->scanInBufferRange(
    Regex(u"\\S"),
    screenLineBufferRange,
    [&](TextBuffer::SearchCallbackArgument &argument) {
      firstCharacterColumn = argument.range.start.column;
      argument.stop();
    }
  );

  if (
    firstCharacterColumn &&
    *firstCharacterColumn != this->getBufferColumn()
  ) {
    targetBufferColumn = *firstCharacterColumn;
  } else {
    targetBufferColumn = screenLineBufferRange.start.column;
  }

  this->setBufferPosition({
    screenLineBufferRange.start.row,
    targetBufferColumn
  });
}

void Cursor::moveToEndOfScreenLine() {
  this->setScreenPosition(Point(this->getScreenRow(), INFINITY));
}

void Cursor::moveToEndOfLine() {
  this->setBufferPosition(Point(this->getBufferRow(), INFINITY));
}

void Cursor::moveToBeginningOfWord() {
  this->setBufferPosition(this->getBeginningOfCurrentWordBufferPosition());
}

void Cursor::moveToEndOfWord() {
  const Point position = this->getEndOfCurrentWordBufferPosition();
  /* if (position) */ this->setBufferPosition(position);
}

void Cursor::moveToBeginningOfNextWord() {
  const Point position = this->getBeginningOfNextWordBufferPosition();
  /* if (position) */ this->setBufferPosition(position);
}

void Cursor::moveToPreviousWordBoundary() {
  const Point position = this->getPreviousWordBoundaryBufferPosition();
  /* if (position) */ this->setBufferPosition(position);
}

void Cursor::moveToNextWordBoundary() {
  const Point position = this->getNextWordBoundaryBufferPosition();
  /* if (position) */ this->setBufferPosition(position);
}

void Cursor::moveToPreviousSubwordBoundary() {
  const Regex wordRegex = this->subwordRegExp(true);
  const Point position = this->getPreviousWordBoundaryBufferPosition(&wordRegex);
  /* if (position) */ this->setBufferPosition(position);
}

void Cursor::moveToNextSubwordBoundary() {
  const Regex wordRegex = this->subwordRegExp();
  const Point position = this->getNextWordBoundaryBufferPosition(&wordRegex);
  /* if (position) */ this->setBufferPosition(position);
}

void Cursor::skipLeadingWhitespace() {
  const Point position = this->getBufferPosition();
  const Range scanRange = this->getCurrentLineBufferRange();
  Point endOfLeadingWhitespace;
  this->editor->scanInBufferRange(Regex(u"^[ \t]*"), scanRange, [&](TextBuffer::SearchCallbackArgument &argument) {
    endOfLeadingWhitespace = argument.range.end;
  });

  if (endOfLeadingWhitespace.isGreaterThan(position))
    this->setBufferPosition(endOfLeadingWhitespace);
}

void Cursor::moveToBeginningOfNextParagraph() {
  const Point position = this->getBeginningOfNextParagraphBufferPosition();
  /* if (position) */ this->setBufferPosition(position);
}

void Cursor::moveToBeginningOfPreviousParagraph() {
  const Point position = this->getBeginningOfPreviousParagraphBufferPosition();
  /* if (position) */ this->setBufferPosition(position);
}

/*
Section: Local Positions and Ranges
*/

Point Cursor::getPreviousWordBoundaryBufferPosition(const Regex *wordRegex) {
  const Point currentBufferPosition = this->getBufferPosition();
  const optional<double> previousNonBlankRow = this->editor->buffer->previousNonBlankRow(
    currentBufferPosition.row
  );
  const Range scanRange = Range(
    Point(previousNonBlankRow ? *previousNonBlankRow : 0, 0),
    currentBufferPosition
  );

  const auto ranges = this->editor->buffer->findAllInRangeSync(
    wordRegex ? *wordRegex : static_cast<const Regex &>(this->wordRegExp()),
    scanRange
  );

  if (!ranges.empty()) {
    const Range range = ranges[ranges.size() - 1];
    if (
      range.start.row < currentBufferPosition.row &&
      currentBufferPosition.column > 0
    ) {
      return Point(currentBufferPosition.row, 0);
    } else if (currentBufferPosition.isGreaterThan(range.end)) {
      return range.end;
    } else {
      return range.start;
    }
  } else {
    return currentBufferPosition;
  }
}

Point Cursor::getNextWordBoundaryBufferPosition(const Regex *wordRegex) {
  const Point currentBufferPosition = this->getBufferPosition();
  const Range scanRange = Range(
    currentBufferPosition,
    this->editor->getEofBufferPosition()
  );

  const auto range = this->editor->buffer->findInRangeSync(
    wordRegex ? *wordRegex : static_cast<const Regex &>(this->wordRegExp()),
    scanRange
  );

  if (range) {
    if (range->start.row > currentBufferPosition.row) {
      return Point(range->start.row, 0);
    } else if (currentBufferPosition.isLessThan(range->start)) {
      return range->start;
    } else {
      return range->end;
    }
  } else {
    return currentBufferPosition;
  }
}

Point Cursor::getBeginningOfCurrentWordBufferPosition(/* options = {} */) {
  const bool allowPrevious = /* options.allowPrevious !== false */ true;
  const Point position = this->getBufferPosition();

  const Range scanRange = allowPrevious
    ? Range(Point(position.row - 1, 0), position)
    : Range(Point(position.row, 0), position);

  const auto ranges = this->editor->buffer->findAllInRangeSync(
    /* options.wordRegex || */ this->wordRegExp( /* options */ ),
    scanRange
  );

  optional<Point> result;
  for (auto range : ranges) {
    if (position.isLessThanOrEqual(range.start)) break;
    if (allowPrevious || position.isLessThanOrEqual(range.end))
      result = optional<Point>(range.start);
  }

  return result ? *result : (allowPrevious ? Point(0, 0) : position);
}

Point Cursor::getEndOfCurrentWordBufferPosition(/* options = {} */) {
  const bool allowNext = /*options.allowNext !== false*/ true;
  const Point position = this->getBufferPosition();

  const Range scanRange = allowNext
    ? Range(position, Point(position.row + 2, 0))
    : Range(position, Point(position.row, INFINITY));

  const auto ranges = this->editor->buffer->findAllInRangeSync(
    /* options.wordRegex || */ this->wordRegExp(/* options */),
    scanRange
  );

  for (auto range : ranges) {
    if (position.isLessThan(range.start) && !allowNext) break;
    if (position.isLessThan(range.end)) return Point(range.end);
  }

  return allowNext ? this->editor->getEofBufferPosition() : position;
}

Point Cursor::getBeginningOfNextWordBufferPosition(/* options = {} */) {
  const Point currentBufferPosition = this->getBufferPosition();
  const Point start = this->isInsideWord(/* options */)
    ? this->getEndOfCurrentWordBufferPosition(/* options */)
    : currentBufferPosition;
  const Range scanRange = Range(start, this->editor->getEofBufferPosition());

  optional<Point> beginningOfNextWordPosition;
  this->editor->scanInBufferRange(
    /* options.wordRegex || */ this->wordRegExp(),
    scanRange,
    [&](TextBuffer::SearchCallbackArgument &argument) {
      beginningOfNextWordPosition = argument.range.start;
      argument.stop();
    }
  );

  return beginningOfNextWordPosition ? *beginningOfNextWordPosition : currentBufferPosition;
}

Range Cursor::getCurrentWordBufferRange(const Regex *wordRegex, bool includeNonWordCharacters) {
  const Point position = this->getBufferPosition();
  const auto ranges = this->editor->buffer->findAllInRangeSync(
    wordRegex ? *wordRegex : static_cast<const Regex &>(this->wordRegExp(includeNonWordCharacters)),
    Range(Point(position.row, 0), Point(position.row, INFINITY))
  );
  auto range = std::find_if(ranges.begin(), ranges.end(), [&](const Range &range) {
    return range.end.column >= position.column &&
      range.start.column <= position.column;
  });
  return range != ranges.end() ? Range(*range) : Range(position, position);
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
  if (this->selection) this->selection->clear(autoscroll);
}

Regex Cursor::wordRegExp(bool includeNonWordCharacters) {
  const std::u16string nonWordCharacters = escapeRegExp(this->getNonWordCharacters());
  std::u16string source = u"^[\t ]*$|[^\\s" + nonWordCharacters + u"]+";
  if (includeNonWordCharacters) {
    source += u"|[" + nonWordCharacters + u"]+";
  }
  return Regex(source);
}

Regex Cursor::subwordRegExp(bool backwards) {
  const char16_t *nonWordCharacters = this->getNonWordCharacters();
  const std::u16string lowercaseLetters = u"a-z\\u00DF-\\u00F6\\u00F8-\\u00FF";
  const std::u16string uppercaseLetters = u"A-Z\\u00C0-\\u00D6\\u00D8-\\u00DE";
  const std::u16string snakeCamelSegment = u"[" + uppercaseLetters + u"]?[" + lowercaseLetters + u"]+";
  std::vector<std::u16string> segments = {
    u"^[\t ]+",
    u"[\t ]+$",
    u"[" + uppercaseLetters + u"]+(?![" + lowercaseLetters + u"])",
    u"\\d+"
  };
  if (backwards) {
    segments.push_back(snakeCamelSegment + u"_*");
    segments.push_back(u"[" + escapeRegExp(nonWordCharacters) + u"]+\\s*");
  } else {
    segments.push_back(u"_*" + snakeCamelSegment);
    segments.push_back(u"\\s*[" + escapeRegExp(nonWordCharacters) + u"]+");
  }
  segments.push_back(u"_+");
  return Regex(join(segments, u"|"));
}

/*
Section: Private
*/

const char16_t *Cursor::getNonWordCharacters() {
  return this->editor->getNonWordCharacters(this->getBufferPosition());
}

void Cursor::changePosition(optional<bool> options_autoscroll, std::function<void()> fn) {
  this->clearSelection(false);
  fn();
  this->goalColumn = optional<double>();
  const bool autoscroll =
    options_autoscroll
      ? *options_autoscroll
      : this->isLastCursor();
  if (autoscroll) this->autoscroll();
}

Range Cursor::getScreenRange() {
  //const auto [row, column] = this->getScreenPosition();
  const Point screenPosition = this->getScreenPosition();
  const double row = screenPosition.row, column = screenPosition.column;
  return Range(Point(row, column), Point(row, column + 1));
}

void Cursor::autoscroll() {
  this->editor->scrollToScreenRange(this->getScreenRange(), false);
}

Point Cursor::getBeginningOfNextParagraphBufferPosition() {
  const Point start = this->getBufferPosition();
  const Point eof = this->editor->getEofBufferPosition();
  const Range scanRange = Range(start, eof);

  const double row = eof.row, column = eof.column;
  Point position = Point(row, column - 1);

  this->editor->scanInBufferRange(
    EmptyLineRegExp,
    scanRange,
    [&](TextBuffer::SearchCallbackArgument &argument) {
      position = argument.range.start.traverse(Point(1, 0));
      if (!position.isEqual(start)) argument.stop();
    }
  );
  return position;
}

Point Cursor::getBeginningOfPreviousParagraphBufferPosition() {
  const Point start = this->getBufferPosition();

  const double row = start.row, column = start.column;
  const Range scanRange = Range(Point(row - 1, column), Point(0, 0));
  Point position = Point(0, 0);
  this->editor->backwardsScanInBufferRange(
    EmptyLineRegExp,
    scanRange,
    [&](TextBuffer::SearchCallbackArgument &argument) {
      position = argument.range.start.traverse(Point(1, 0));
      if (!position.isEqual(start)) argument.stop();
    }
  );
  return position;
}
