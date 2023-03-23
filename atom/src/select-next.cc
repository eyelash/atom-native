#include "select-next.h"
#include "text-editor.h"
#include "selection.h"
#include <helpers.h>

SelectNext::SelectNext(TextEditor *editor) {
  this->editor = editor;
  this->wordSelected = false;
}

SelectNext::~SelectNext() {}

void SelectNext::findAndSelectNext() {
  if (this->editor->getLastSelection()->isEmpty()) {
    return this->selectWord();
  } else {
    return this->selectNextOccurrence();
  }
}

void SelectNext::selectWord() {
  this->editor->selectWordsContainingCursors();
  Selection *lastSelection = this->editor->getLastSelection();
  if (this->wordSelected = this->isWordSelected(lastSelection)) {
    /*disposables = new CompositeDisposable();
    clearWordSelected = () => {
      this.wordSelected = null;
      return disposables.dispose();
    };
    disposables.add(lastSelection.onDidChangeRange(clearWordSelected));
    return disposables.add(lastSelection.onDidDestroy(clearWordSelected));*/
  }
}

void SelectNext::selectNextOccurrence(/* options = {} */) {
  Point startingRange = /* (ref = options.start) != null ? ref : */ this->editor->getSelectedBufferRange().end;
  auto range = this->findNextOccurrence({startingRange, this->editor->getEofBufferPosition()});
  if (!range) {
    range = this->findNextOccurrence({{0, 0}, this->editor->getSelections()[0]->getBufferRange().start});
  }
  if (range) {
    return this->addSelection(*range);
  }
}

optional<Range> SelectNext::findNextOccurrence(Range scanRange) {
  optional<Range> foundRange;
  this->scanForNextOccurrence(scanRange, [&](TextBuffer::SearchCallbackArgument &argument) {
    foundRange = argument.range;
    return argument.stop();
  });
  return foundRange;
}

void SelectNext::addSelection(Range range) {
  const bool reversed = this->editor->getLastSelection()->isReversed();
  Selection *selection = this->editor->addSelectionForBufferRange(range /* , {reversed} */);
  //return this->updateSavedSelections(selection);
}

void SelectNext::scanForNextOccurrence(Range range, TextBuffer::ScanIterator callback) {
  Selection *selection = this->editor->getLastSelection();
  std::u16string text = escapeRegExp(selection->getText());
  if (this->wordSelected) {
    //nonWordCharacters = atom.config.get('editor.nonWordCharacters');
    const char16_t *nonWordCharacters = this->editor->getNonWordCharacters(Point());
    text = u"(^|[ \t" + escapeRegExp(nonWordCharacters) + u"]+)" + text + u"(?=$|[\\s" + escapeRegExp(nonWordCharacters) + u"]+)";
  }
  return this->editor->scanInBufferRange(Regex(text), range, [&](TextBuffer::SearchCallbackArgument &result) {
    const std::u16string matchText = result.getMatchText();
    Regex::MatchData matchData(result.regex);
    const auto match = result.regex.match(matchText, matchData);
    if (match && matchData.size() >= 2) {
      const double prefixLength = matchData[1].end_offset - matchData[1].start_offset;
      result.range = result.range.translate({0, prefixLength}, {0, 0});
    }
    return callback(result);
  });
}

bool SelectNext::isNonWordCharacter(const std::u16string &character) {
  //nonWordCharacters = atom.config.get('editor.nonWordCharacters');
  const char16_t *nonWordCharacters = this->editor->getNonWordCharacters(Point());
  return Regex(u"[ \t" + escapeRegExp(nonWordCharacters) + u"]").match(character);
}

bool SelectNext::isNonWordCharacterToTheLeft(Selection *selection) {
  const Point selectionStart = selection->getBufferRange().start;
  const Range range = Range::fromPointWithDelta(selectionStart, 0, -1);
  return this->isNonWordCharacter(this->editor->getTextInBufferRange(range));
}

bool SelectNext::isNonWordCharacterToTheRight(Selection *selection) {
  const Point selectionEnd = selection->getBufferRange().end;
  const Range range = Range::fromPointWithDelta(selectionEnd, 0, 1);
  return this->isNonWordCharacter(this->editor->getTextInBufferRange(range));
}

bool SelectNext::isWordSelected(Selection *selection) {
  if (selection->getBufferRange().isSingleLine()) {
    const Range selectionRange = selection->getBufferRange();
    const Range lineRange = this->editor->bufferRangeForBufferRow(selectionRange.start.row);
    const bool nonWordCharacterToTheLeft = selectionRange.start == lineRange.start || this->isNonWordCharacterToTheLeft(selection);
    const bool nonWordCharacterToTheRight = selectionRange.end == lineRange.end || this->isNonWordCharacterToTheRight(selection);
    const bool containsOnlyWordCharacters = !this->isNonWordCharacter(selection->getText());
    return nonWordCharacterToTheLeft && nonWordCharacterToTheRight && containsOnlyWordCharacters;
  } else {
    return false;
  }
}
