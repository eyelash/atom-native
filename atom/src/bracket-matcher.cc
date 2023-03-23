#include "bracket-matcher.h"
#include "text-editor.h"
#include "selection.h"
#include <initializer_list>

static bool endsWithEscapeCharacter(const std::u16string &);
static bool endsWithEscapeSequence(const std::u16string &);

static const std::vector<const char16_t *> autocompleteCharacters = {
  u"()",
  u"[]",
  u"{}",
  u"\"\"",
  u"''",
  u"``",
  u"“”",
  u"‘’",
  u"«»",
  u"‹›"
};

static const std::vector<const char16_t *> pairsWithExtraNewline = {
  u"()",
  u"[]",
  u"{}"
};

void MatchManager::appendPair(std::unordered_map<char16_t, char16_t> &pairList, std::pair<char16_t, char16_t> item) {
  pairList.insert(item);
}

static std::pair<char16_t, char16_t> split(const char16_t *s) {
  return {s[0], s[1]};
}

void MatchManager::processAutoPairs(const std::vector<const char16_t *> &autocompletePairs, std::unordered_map<char16_t, char16_t> &pairedList, std::function<std::pair<char16_t, char16_t>(std::pair<char16_t, char16_t>)> dataFun) {
  if (autocompletePairs.size()) {
    for (const char16_t *autocompletePair : autocompletePairs) {
      const auto pairArray = split(autocompletePair);
      this->appendPair(pairedList, dataFun(pairArray));
    }
  }
}

void MatchManager::updateConfig() {
  using pair = std::pair<char16_t, char16_t>;
  this->processAutoPairs(::autocompleteCharacters, this->pairedCharacters, [](pair x) -> pair { return {std::get<0>(x), std::get<1>(x)}; });
  this->processAutoPairs(::autocompleteCharacters, this->pairedCharactersInverse, [](pair x) -> pair { return {std::get<1>(x), std::get<0>(x)}; });
  this->processAutoPairs(::pairsWithExtraNewline, this->pairsWithExtraNewline, [](pair x) -> pair { return {std::get<0>(x), std::get<1>(x)}; });
  /*for (let startPair in this.pairedCharacters) {
    const endPair = this.pairedCharacters[startPair]
    this.pairRegexes[startPair] = new RegExp(`[${_.escapeRegExp(startPair + endPair)}]`, 'g')
  }*/
}

MatchManager::MatchManager(TextEditor *editor) {
  this->editor = editor;

  this->updateConfig();

  this->changeBracketsMode = false;
}

BracketMatcher::BracketMatcher(TextEditor *editor) {
  this->editor = editor;
  this->matchManager = new MatchManager(editor);
}

BracketMatcher::~BracketMatcher() {
  delete this->matchManager;
}

void BracketMatcher::insertText(const std::u16string &text, bool groupUndo) {
  if (text.empty()) return this->editor->insertText(text, groupUndo);
  //if ((options && options.select) || (options && options.undo === 'skip')) return true

  bool autoCompleteOpeningBracket = false;
  DisplayMarker *bracketMarker = nullptr;
  char16_t pair;
  if (this->matchManager->changeBracketsMode) {
    this->matchManager->changeBracketsMode = false;
    /*if (this.isClosingBracket(text)) {
      text = this.matchManager.pairedCharactersInverse[text]
    }
    if (this.isOpeningBracket(text)) {
      this.editor.mutateSelectedText(selection => {
        const selectionText = selection.getText()
        if (this.isOpeningBracket(selectionText)) {
          selection.insertText(text)
        }
        if (this.isClosingBracket(selectionText)) {
          selection.insertText(this.matchManager.pairedCharacters[text])
        }
      })
      return false
    }*/
  }

  if (this->wrapSelectionInBrackets(text)) return;
  if (this->editor->hasMultipleCursors()) return this->editor->insertText(text, groupUndo);

  const Point cursorBufferPosition = this->editor->getCursorBufferPosition();
  const std::u16string previousCharacters = this->editor->getTextInBufferRange({{cursorBufferPosition.row, 0}, cursorBufferPosition});
  const std::u16string nextCharacter = this->editor->getTextInBufferRange({cursorBufferPosition, cursorBufferPosition.traverse({0, 1})});
  const std::u16string previousCharacter = previousCharacters.substr(std::max(previousCharacters.size(), static_cast<size_t>(1)) - 1);

  const bool hasWordAfterCursor = Regex(u"\\w").match(nextCharacter);
  const bool hasWordBeforeCursor = Regex(u"\\w").match(previousCharacter);
  const bool hasQuoteBeforeCursor = this->isQuote(previousCharacter) && (previousCharacter == text);
  const bool hasEscapeCharacterBeforeCursor = endsWithEscapeCharacter(previousCharacters);
  const bool hasEscapeSequenceBeforeCursor = endsWithEscapeSequence(previousCharacters);

  /*if (text === '#' && this.isCursorOnInterpolatedString()) {
    autoCompleteOpeningBracket = this.getScopedSetting('bracket-matcher.autocompleteBrackets') && !hasEscapeCharacterBeforeCursor
    text += '{'
    pair = '}'
  } else */ {
    autoCompleteOpeningBracket = (
      this->isOpeningBracket(text) &&
      !hasWordAfterCursor &&
      //this.getScopedSetting('bracket-matcher.autocompleteBrackets') &&
      !(this->isQuote(text) && (hasWordBeforeCursor || hasQuoteBeforeCursor || hasEscapeSequenceBeforeCursor)) &&
      !hasEscapeCharacterBeforeCursor
    );
    if (autoCompleteOpeningBracket) pair = this->matchManager->pairedCharacters[text[0]];
  }

  bool skipOverExistingClosingBracket = false;
  if (this->isClosingBracket(text) && (nextCharacter == text) && !hasEscapeCharacterBeforeCursor) {
    //bracketMarker = this.bracketMarkers.find(marker => marker.isValid() && marker.getBufferRange().end.isEqual(cursorBufferPosition))
    //if (bracketMarker || this.getScopedSetting('bracket-matcher.alwaysSkipClosingPairs')) {
      skipOverExistingClosingBracket = true;
    //}
  }

  if (skipOverExistingClosingBracket) {
    //if (bracketMarker) bracketMarker.destroy()
    //_.remove(this.bracketMarkers, bracketMarker)
    this->editor->moveRight();
    return;
  } else if (autoCompleteOpeningBracket) {
    this->editor->transact([&]() {
      this->editor->insertText(text + pair);
      this->editor->moveLeft();
    });
    //const Range range = {cursorBufferPosition, cursorBufferPosition.traverse({0, static_cast<double>(text.size())})};
    //this.bracketMarkers.push(this.editor.markBufferRange(range))
    return;
  }
  this->editor->insertText(text, groupUndo);
}

void BracketMatcher::insertNewline() {
  if (this->editor->hasMultipleCursors()) return this->editor->insertNewline();
  if (!this->editor->getLastSelection()->isEmpty()) return this->editor->insertNewline();

  const Point cursorBufferPosition = this->editor->getCursorBufferPosition();
  const std::u16string previousCharacters = this->editor->getTextInBufferRange({{cursorBufferPosition.row, 0}, cursorBufferPosition});
  const std::u16string nextCharacter = this->editor->getTextInBufferRange({cursorBufferPosition, cursorBufferPosition.traverse({0, 1})});
  const std::u16string previousCharacter = previousCharacters.substr(std::max(previousCharacters.size(), static_cast<size_t>(1)) - 1);
  const bool hasEscapeCharacterBeforeCursor = endsWithEscapeCharacter(previousCharacters);

  if (
    previousCharacter.size() == 1 &&
    nextCharacter.size() == 1 &&
    this->matchManager->pairsWithExtraNewline.count(previousCharacter[0]) &&
    this->matchManager->pairsWithExtraNewline[previousCharacter[0]] == nextCharacter[0] &&
    !hasEscapeCharacterBeforeCursor
  ) {
    this->editor->transact([&]() {
      this->editor->insertText(u"\n\n");
      this->editor->moveUp();
      //if (this.getScopedSetting('editor.autoIndent')) {
        const double cursorRow = this->editor->getCursorBufferPosition().row;
        this->editor->autoIndentBufferRows(cursorRow, cursorRow + 1);
      //}
    });
    return;
  }
  this->editor->insertNewline();
}

void BracketMatcher::backspace() {
  if (this->editor->hasMultipleCursors()) return this->editor->backspace();
  if (!this->editor->getLastSelection()->isEmpty()) return this->editor->backspace();

  const Point cursorBufferPosition = this->editor->getCursorBufferPosition();
  const std::u16string previousCharacters = this->editor->getTextInBufferRange({{cursorBufferPosition.row, 0}, cursorBufferPosition});
  const std::u16string nextCharacter = this->editor->getTextInBufferRange({cursorBufferPosition, cursorBufferPosition.traverse({0, 1})});
  const std::u16string previousCharacter = previousCharacters.substr(std::max(previousCharacters.size(), static_cast<size_t>(1)) - 1);
  const bool hasEscapeCharacterBeforeCursor = endsWithEscapeCharacter(previousCharacters);

  if (
    previousCharacter.size() == 1 &&
    nextCharacter.size() == 1 &&
    this->matchManager->pairedCharacters.count(previousCharacter[0]) &&
    this->matchManager->pairedCharacters[previousCharacter[0]] == nextCharacter[0] &&
    !hasEscapeCharacterBeforeCursor
    // && this.getScopedSetting('bracket-matcher.autocompleteBrackets')
  ) {
    this->editor->transact([&]() {
      this->editor->moveLeft();
      this->editor->delete_();
      this->editor->delete_();
    });
    return;
  }
  this->editor->backspace();
}

bool BracketMatcher::wrapSelectionInBrackets(const std::u16string &bracket) {
  char16_t pair;
  /*if (bracket == u"#") {
    if (!this.isCursorOnInterpolatedString()) return false;
    bracket = '#{';
    pair = u'}';
  } else */ {
    if (!this->isOpeningBracket(bracket)) return false;
    pair = this->matchManager->pairedCharacters[bracket[0]];
  }

  //if (!this.editor.selections.some(s => !s.isEmpty())) return false;
  //if (!this.getScopedSetting('bracket-matcher.wrapSelectionsInBrackets')) return false;

  bool selectionWrapped = false;
  /*this->editor->mutateSelectedText([&](Selection *selection) {
    Point selectionEnd;
    if (selection->isEmpty()) return;

    // Don't wrap in #{} if the selection spans more than one line
    if ((bracket === '#{') && !selection.getBufferRange().isSingleLine()) return

    selectionWrapped = true;
    const Range range = selection->getBufferRange();
    const options = {reversed: selection.isReversed()}
    selection.insertText(`${bracket}${selection.getText()}${pair}`)
    const Point selectionStart = range.start.traverse({0, static_cast<double>(bracket.size())});
    if (range.start.row == range.end.row) {
      selectionEnd = range.end.traverse({0, static_cast<double>(bracket.size())});
    } else {
      selectionEnd = range.end;
    }
    selection->setBufferRange({selectionStart, selectionEnd}, options);
  });*/

  return selectionWrapped;
}

bool BracketMatcher::isQuote(const std::u16string &string) {
  return Regex(u"['\"`]").match(string);
}

bool BracketMatcher::isOpeningBracket(const std::u16string &string) {
  return string.size() == 1 && this->matchManager->pairedCharacters.count(string[0]);
}

bool BracketMatcher::isClosingBracket(const std::u16string &string) {
  return string.size() == 1 && this->matchManager->pairedCharactersInverse.count(string[0]);
}

static const Regex BACKSLASHES_REGEX = Regex(u"(\\\\+)$");
static const Regex ESCAPE_SEQUENCE_REGEX = Regex(u"(\\\\+)[^\\\\]$");

// odd number of backslashes
static bool endsWithEscapeCharacter(const std::u16string &string) {
  Regex::MatchData backslashesMatchData(BACKSLASHES_REGEX);
  const auto backslashesMatch = BACKSLASHES_REGEX.match(string, backslashesMatchData);
  return backslashesMatch && (backslashesMatchData[1].end_offset - backslashesMatchData[1].start_offset) % 2 == 1;
}

// even number of backslashes or odd number of backslashes followed by another character
static bool endsWithEscapeSequence(const std::u16string &string) {
  Regex::MatchData backslashesMatchData(BACKSLASHES_REGEX);
  Regex::MatchData escapeSequenceMatchData(ESCAPE_SEQUENCE_REGEX);
  const auto backslashesMatch = BACKSLASHES_REGEX.match(string, backslashesMatchData);
  const auto escapeSequenceMatch = ESCAPE_SEQUENCE_REGEX.match(string, escapeSequenceMatchData);
  return (
    (escapeSequenceMatch && (escapeSequenceMatchData[1].end_offset - escapeSequenceMatchData[1].start_offset) % 2 == 1) ||
    (backslashesMatch && (backslashesMatchData[1].end_offset - backslashesMatchData[1].start_offset) % 2 == 0)
  );
}
