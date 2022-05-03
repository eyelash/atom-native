#include "screen-line-builder.h"
#include "text-buffer.h"

constexpr int32_t HARD_TAB = 1 << 0;
constexpr int32_t LEADING_WHITESPACE = 1 << 2;
constexpr int32_t TRAILING_WHITESPACE = 1 << 3;
constexpr int32_t INVISIBLE_CHARACTER = 1 << 4;
constexpr int32_t INDENT_GUIDE = 1 << 5;
constexpr int32_t LINE_ENDING = 1 << 6;
constexpr int32_t FOLD = 1 << 7;

unsigned nextScreenLineId = 1;

ScreenLineBuilder::ScreenLineBuilder(DisplayLayer *displayLayer) :
  displayLayer{displayLayer} {}

ScreenLineBuilder::~ScreenLineBuilder() {}

std::vector<DisplayLayer::ScreenLine> ScreenLineBuilder::buildScreenLines(double startScreenRow, double endScreenRow) {
  this->requestedStartScreenRow = startScreenRow;
  this->requestedEndScreenRow = endScreenRow;
  this->displayLayer->populateSpatialIndexIfNeeded(this->displayLayer->buffer->getLineCount(), endScreenRow);

  this->bufferPosition = Point(
    this->displayLayer->findBoundaryPrecedingBufferRow(
      this->displayLayer->translateScreenPositionWithSpatialIndex(Point(startScreenRow, 0)).row
    ),
    0
  );

  this->screenRow = this->displayLayer->translateBufferPositionWithSpatialIndex(Point(this->bufferPosition.row, 0)).row;

  const double endBufferRow = this->displayLayer->translateScreenPositionWithSpatialIndex(Point(endScreenRow, INFINITY)).row;

  bool didSeekDecorationIterator = false;
  auto decorationIterator = this->displayLayer->buffer->languageMode->buildHighlightIterator();
  auto hunks = this->displayLayer->spatialIndex->grab_changes_in_new_range(Point(this->screenRow, 0), Point(endScreenRow, 0));
  double hunkIndex = 0;

  this->containingScopeIds = std::vector<int32_t>();
  this->scopeIdsToReopen = std::vector<int32_t>();
  this->screenLines = std::vector<DisplayLayer::ScreenLine>();
  this->bufferPosition.column = 0;
  this->beginLine();

  // Loop through all characters spanning the given screen row range, building
  // up screen lines based on the contents of the spatial index and the
  // buffer.
  screenRowLoop:
  while (this->screenRow < endScreenRow) {
    optional<DisplayLayer::ScreenLine> cachedScreenLine = this->screenRow < this->displayLayer->cachedScreenLines.size() ? this->displayLayer->cachedScreenLines[this->screenRow] : optional<DisplayLayer::ScreenLine>();
    if (cachedScreenLine) {
      this->pushScreenLine(*cachedScreenLine);

      Patch::Change *nextHunk = &hunks[hunkIndex];
      while (hunkIndex < hunks.size() && nextHunk->new_start.row <= this->screenRow) {
        if (nextHunk->new_start.row == this->screenRow) {
          if (nextHunk->new_end.row > nextHunk->new_start.row) {
            this->screenRow++;
            this->bufferPosition.column = nextHunk->old_end.column;
            hunkIndex++;
            goto continueScreenRowLoop;
          } else {
            this->bufferPosition.row = nextHunk->old_end.row;
            this->bufferPosition.column = nextHunk->old_end.column;
          }
        }

        hunkIndex++;
        nextHunk = &hunks[hunkIndex];
      }

      this->screenRow++;
      this->screenColumn = 0;
      this->bufferPosition.row++;
      this->bufferPosition.column = 0;
      continue;
    }

    this->currentBuiltInClassNameFlags = 0;
    this->bufferLineLength = *this->displayLayer->buffer->lineLengthForRow(this->bufferPosition.row);

    if (this->bufferPosition.row > this->displayLayer->buffer->getLastRow()) break;
    this->trailingWhitespaceStartColumn = this->displayLayer->findTrailingWhitespaceStartColumn(this->bufferPosition.row);
    this->inLeadingWhitespace = true;
    this->inTrailingWhitespace = false;

    if (!didSeekDecorationIterator || this->compareBufferPosition(decorationIterator->getPosition()) > 0) {
      didSeekDecorationIterator = true;
      this->scopeIdsToReopen = decorationIterator->seek(this->bufferPosition, endBufferRow);
    }

    {
      optional<DisplayLayer::ScreenLine> prevCachedScreenLine = this->screenRow - 1 >= 0 && this->screenRow - 1 < this->displayLayer->cachedScreenLines.size() ? this->displayLayer->cachedScreenLines[this->screenRow - 1] : optional<DisplayLayer::ScreenLine>();
      if (prevCachedScreenLine && prevCachedScreenLine->softWrapIndent >= 0) {
        this->inLeadingWhitespace = false;
        if (prevCachedScreenLine->softWrapIndent > 0) this->emitIndentWhitespace(prevCachedScreenLine->softWrapIndent);
      }
    }

    // This loop may visit multiple buffer rows if there are folds and
    // multiple screen rows if there are soft wraps.
    while (this->bufferPosition.column <= this->bufferLineLength) {
      // Handle folds or soft wraps at the current position.
      Patch::Change *nextHunk = &hunks[hunkIndex];
      while (hunkIndex < hunks.size() && nextHunk->old_start.row == this->bufferPosition.row && nextHunk->old_start.column == this->bufferPosition.column) {
        if (this->displayLayer->isSoftWrapHunk(*nextHunk)) {
          this->emitSoftWrap(*nextHunk);
          if (this->screenRow == endScreenRow) {
            goto breakScreenRowLoop;
          }
        } else {
          this->emitFold(*nextHunk, *decorationIterator, endBufferRow);
        }

        hunkIndex++;
        nextHunk = &hunks[hunkIndex];
      }

      char16_t nextCharacter = this->displayLayer->buffer->getCharacterAtPosition(this->bufferPosition);
      if (this->bufferPosition.column >= this->trailingWhitespaceStartColumn) {
        this->inTrailingWhitespace = true;
        this->inLeadingWhitespace = false;
      } else if (nextCharacter != u' ' && nextCharacter != u'\t') {
        this->inLeadingWhitespace = false;
      }

      // Compute a token flags describing built-in decorations for the token
      // containing the next character
      int32_t previousBuiltInTagFlags = this->currentBuiltInClassNameFlags;
      this->updateCurrentTokenFlags(nextCharacter);

      if (this->emitBuiltInTagBoundary) {
        this->emitCloseTag(this->getBuiltInScopeId(previousBuiltInTagFlags));
      }

      this->emitDecorationBoundaries(*decorationIterator);

      // Are we at the end of the line?
      if (this->bufferPosition.column == this->bufferLineLength) {
        this->emitLineEnding();
        break;
      }

      if (this->emitBuiltInTagBoundary) {
        this->emitOpenTag(this->getBuiltInScopeId(this->currentBuiltInClassNameFlags));
      }

      // Emit the next character, handling hard tabs whitespace invisibles
      // specially.
      if (nextCharacter == u'\t') {
        this->emitHardTab();
      } else if ((this->inLeadingWhitespace || this->inTrailingWhitespace) &&
                  nextCharacter == u' ' && this->displayLayer->invisibles.space) {
        this->emitText(this->displayLayer->invisibles.space);
      } else {
        this->emitText({nextCharacter});
      }
      this->bufferPosition.column++;
    }
    continueScreenRowLoop:
    continue;
  }
  breakScreenRowLoop:

  return this->screenLines;
}

double ScreenLineBuilder::getBuiltInScopeId(int32_t flags) {
  if (flags == 0) return 0;

  double scopeId = this->displayLayer->getBuiltInScopeId(flags);
  if (scopeId == -1) {
    std::u16string className = u"";
    if (flags & INVISIBLE_CHARACTER) className += u"invisible-character ";
    if (flags & HARD_TAB) className += u"hard-tab ";
    if (flags & LEADING_WHITESPACE) className += u"leading-whitespace ";
    if (flags & TRAILING_WHITESPACE) className += u"trailing-whitespace ";
    if (flags & LINE_ENDING) className += u"eol ";
    if (flags & INDENT_GUIDE) className += u"indent-guide ";
    if (flags & FOLD) className += u"fold-marker ";
    if (!className.empty()) className.pop_back();
    scopeId = this->displayLayer->registerBuiltInScope(flags, className);
  }
  return scopeId;
}

void ScreenLineBuilder::beginLine() {
  this->currentScreenLineText = u"";
  this->currentScreenLineTags = std::vector<int32_t>();
  this->screenColumn = 0;
  this->currentTokenLength = 0;
}

void ScreenLineBuilder::updateCurrentTokenFlags(char16_t nextCharacter) {
  const int32_t previousBuiltInTagFlags = this->currentBuiltInClassNameFlags;
  this->currentBuiltInClassNameFlags = 0;
  this->emitBuiltInTagBoundary = false;

  if (nextCharacter == u' ' || nextCharacter == u'\t') {
    const bool showIndentGuides = this->displayLayer->showIndentGuides && (this->inLeadingWhitespace || this->trailingWhitespaceStartColumn == 0);
    if (this->inLeadingWhitespace) this->currentBuiltInClassNameFlags |= LEADING_WHITESPACE;
    if (this->inTrailingWhitespace) this->currentBuiltInClassNameFlags |= TRAILING_WHITESPACE;

    if (nextCharacter == u' ') {
      if ((this->inLeadingWhitespace || this->inTrailingWhitespace) && this->displayLayer->invisibles.space) {
        this->currentBuiltInClassNameFlags |= INVISIBLE_CHARACTER;
      }

      if (showIndentGuides) {
        this->currentBuiltInClassNameFlags |= INDENT_GUIDE;
        if (std::fmod(this->screenColumn, this->displayLayer->tabLength) == 0) this->emitBuiltInTagBoundary = true;
      }
    } else { // nextCharacter == \t
      this->currentBuiltInClassNameFlags |= HARD_TAB;
      if (this->displayLayer->invisibles.tab) this->currentBuiltInClassNameFlags |= INVISIBLE_CHARACTER;
      if (showIndentGuides && std::fmod(this->screenColumn, this->displayLayer->tabLength) == 0) {
        this->currentBuiltInClassNameFlags |= INDENT_GUIDE;
      }

      this->emitBuiltInTagBoundary = true;
    }
  }

  if (!this->emitBuiltInTagBoundary) {
    this->emitBuiltInTagBoundary = this->currentBuiltInClassNameFlags != previousBuiltInTagFlags;
  }
}

void ScreenLineBuilder::emitDecorationBoundaries(LanguageMode::HighlightIterator &decorationIterator) {
  while (this->compareBufferPosition(decorationIterator.getPosition()) == 0) {
    auto closeScopeIds = decorationIterator.getCloseScopeIds();
    for (double i = 0, n = closeScopeIds.size(); i < n; i++) {
      this->emitCloseTag(closeScopeIds[i]);
    }

    auto openScopeIds = decorationIterator.getOpenScopeIds();
    for (double i = 0, n = openScopeIds.size(); i < n; i++) {
      this->emitOpenTag(openScopeIds[i]);
    }

    decorationIterator.moveToSuccessor();
  }
}

void ScreenLineBuilder::emitFold(const Patch::Change &nextHunk, LanguageMode::HighlightIterator &decorationIterator, double endBufferRow) {
  this->emitCloseTag(this->getBuiltInScopeId(this->currentBuiltInClassNameFlags));
  this->currentBuiltInClassNameFlags = 0;

  this->closeContainingScopes();
  this->scopeIdsToReopen.resize(0);

  this->emitOpenTag(this->getBuiltInScopeId(FOLD));
  this->emitText({this->displayLayer->foldCharacter});
  this->emitCloseTag(this->getBuiltInScopeId(FOLD));

  this->bufferPosition.row = nextHunk.old_end.row;
  this->bufferPosition.column = nextHunk.old_end.column;

  this->scopeIdsToReopen = decorationIterator.seek(this->bufferPosition, endBufferRow);

  this->bufferLineLength = *this->displayLayer->buffer->lineLengthForRow(this->bufferPosition.row);
  this->trailingWhitespaceStartColumn = this->displayLayer->findTrailingWhitespaceStartColumn(this->bufferPosition.row);
}

void ScreenLineBuilder::emitSoftWrap(const Patch::Change &nextHunk) {
  this->emitCloseTag(this->getBuiltInScopeId(this->currentBuiltInClassNameFlags));
  this->currentBuiltInClassNameFlags = 0;
  this->closeContainingScopes();
  this->emitNewline(nextHunk.new_end.column);
  this->emitIndentWhitespace(nextHunk.new_end.column);
}

void ScreenLineBuilder::emitLineEnding() {
  this->emitCloseTag(this->getBuiltInScopeId(this->currentBuiltInClassNameFlags));

  auto lineEnding = this->displayLayer->buffer->lineEndingForRow(this->bufferPosition.row);
  auto eolInvisible = this->displayLayer->eolInvisibles[lineEnding];
  if (eolInvisible) {
    int32_t eolFlags = INVISIBLE_CHARACTER | LINE_ENDING;
    if (this->bufferLineLength == 0 && this->displayLayer->showIndentGuides) eolFlags |= INDENT_GUIDE;
    this->emitOpenTag(this->getBuiltInScopeId(eolFlags));
    this->emitText(eolInvisible, false);
    this->emitCloseTag(this->getBuiltInScopeId(eolFlags));
  }

  if (this->bufferLineLength == 0 && this->displayLayer->showIndentGuides) {
    double whitespaceLength = this->displayLayer->leadingWhitespaceLengthForSurroundingLines(this->bufferPosition.row);
    this->emitIndentWhitespace(whitespaceLength);
  }

  this->closeContainingScopes();

  // Ensure empty lines have at least one empty token to make it easier on
  // the caller
  if (this->currentScreenLineTags.size() == 0) this->currentScreenLineTags.push_back(0);
  this->emitNewline();
  this->bufferPosition.row++;
  this->bufferPosition.column = 0;
}

void ScreenLineBuilder::emitNewline(double softWrapIndent) {
  const DisplayLayer::ScreenLine screenLine = {
    nextScreenLineId++,
    this->currentScreenLineText,
    this->currentScreenLineTags,
    softWrapIndent
  };
  this->pushScreenLine(screenLine);
  this->displayLayer->cachedScreenLines[this->screenRow] = screenLine;
  this->screenRow++;
  this->beginLine();
}

void ScreenLineBuilder::emitIndentWhitespace(double endColumn) {
  if (this->displayLayer->showIndentGuides) {
    bool openedIndentGuide = false;
    while (this->screenColumn < endColumn) {
      if (std::fmod(this->screenColumn, this->displayLayer->tabLength) == 0) {
        if (openedIndentGuide) {
          this->emitCloseTag(this->getBuiltInScopeId(INDENT_GUIDE));
        }

        this->emitOpenTag(this->getBuiltInScopeId(INDENT_GUIDE));
        openedIndentGuide = true;
      }
      this->emitText(u" ", false);
    }

    if (openedIndentGuide) this->emitCloseTag(this->getBuiltInScopeId(INDENT_GUIDE));
  } else {
    this->emitText(std::u16string(endColumn - this->screenColumn, u' '), false);
  }
}

void ScreenLineBuilder::emitHardTab() {
  const double distanceToNextTabStop = this->displayLayer->tabLength - std::fmod(this->screenColumn, this->displayLayer->tabLength);
  if (this->displayLayer->invisibles.tab) {
    this->emitText(this->displayLayer->invisibles.tab);
    this->emitText(std::u16string(distanceToNextTabStop - 1, u' '));
  } else {
    this->emitText(std::u16string(distanceToNextTabStop, u' '));
  }
}

void ScreenLineBuilder::emitText(std::u16string text, bool reopenTags) {
  if (reopenTags) this->reopenTags();
  this->currentScreenLineText += text;
  const double length = text.size();
  this->screenColumn += length;
  this->currentTokenLength += length;
}

void ScreenLineBuilder::emitTokenBoundary() {
  if (this->currentTokenLength > 0) {
    this->currentScreenLineTags.push_back(this->currentTokenLength);
    this->currentTokenLength = 0;
  }
}

void ScreenLineBuilder::emitEmptyTokenIfNeeded() {
  const int32_t lastTag = this->currentScreenLineTags[this->currentScreenLineTags.size() - 1];
  if (this->displayLayer->isOpenTag(lastTag)) {
    this->currentScreenLineTags.push_back(0);
  }
}

void ScreenLineBuilder::emitCloseTag(int32_t scopeId) {
  this->emitTokenBoundary();

  if (scopeId == 0) return;

  for (double i = this->scopeIdsToReopen.size() - 1.0; i >= 0; i--) {
    if (this->scopeIdsToReopen[i] == scopeId) {
      this->scopeIdsToReopen.erase(this->scopeIdsToReopen.begin() + i);
      return;
    }
  }

  this->emitEmptyTokenIfNeeded();

  int32_t containingScopeId;
  while (!this->containingScopeIds.empty()) {
    containingScopeId = this->containingScopeIds.back();
    this->containingScopeIds.pop_back();
    this->currentScreenLineTags.push_back(this->displayLayer->closeTagForScopeId(containingScopeId));
    if (containingScopeId == scopeId) {
      return;
    } else {
      this->scopeIdsToReopen.insert(this->scopeIdsToReopen.begin(), containingScopeId);
    }
  }
}

void ScreenLineBuilder::emitOpenTag(double scopeId, bool reopenTags) {
  if (reopenTags) this->reopenTags();
  this->emitTokenBoundary();
  if (scopeId > 0) {
    this->containingScopeIds.push_back(scopeId);
    this->currentScreenLineTags.push_back(this->displayLayer->openTagForScopeId(scopeId));
  }
}

void ScreenLineBuilder::closeContainingScopes() {
  if (this->containingScopeIds.size() > 0) this->emitEmptyTokenIfNeeded();

  for (double i = this->containingScopeIds.size() - 1.0; i >= 0; i--) {
    const int32_t containingScopeId = this->containingScopeIds[i];
    this->currentScreenLineTags.push_back(this->displayLayer->closeTagForScopeId(containingScopeId));
    this->scopeIdsToReopen.insert(this->scopeIdsToReopen.begin(), containingScopeId);
  }
  this->containingScopeIds.resize(0);
}

void ScreenLineBuilder::reopenTags() {
  for (double i = 0, n = this->scopeIdsToReopen.size(); i < n; i++) {
    const int32_t scopeIdToReopen = this->scopeIdsToReopen[i];
    this->containingScopeIds.push_back(scopeIdToReopen);
    this->currentScreenLineTags.push_back(this->displayLayer->openTagForScopeId(scopeIdToReopen));
  }
  this->scopeIdsToReopen.resize(0);
}

void ScreenLineBuilder::pushScreenLine(const DisplayLayer::ScreenLine &screenLine) {
  if (this->requestedStartScreenRow <= this->screenRow && this->screenRow < this->requestedEndScreenRow) {
    this->screenLines.push_back(screenLine);
  }
}

double ScreenLineBuilder::compareBufferPosition(Point position) {
  const double rowComparison = this->bufferPosition.row - position.row;
  return rowComparison == 0 ? (this->bufferPosition.column - position.column) : rowComparison;
}
