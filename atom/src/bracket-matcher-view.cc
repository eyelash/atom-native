#include "bracket-matcher-view.h"
#include "match-manager.h"
#include "text-editor.h"
#include "selection.h"
#include "tree-sitter-language-mode.h"
#include <display-marker.h>

static const double MAX_ROWS_TO_SCAN = 10000;
static const Point ONE_CHAR_FORWARD_TRAVERSAL = Point(0, 1);
static const Point ONE_CHAR_BACKWARD_TRAVERSAL = Point(0, -1);
static const Point TWO_CHARS_BACKWARD_TRAVERSAL = Point(0, -2);
static const Point MAX_ROWS_TO_SCAN_FORWARD_TRAVERSAL = Point(MAX_ROWS_TO_SCAN, 0);
static const Point MAX_ROWS_TO_SCAN_BACKWARD_TRAVERSAL = Point(-MAX_ROWS_TO_SCAN, 0);

BracketMatcherView::BracketMatcherView(TextEditor *editor, MatchManager *matchManager) {
  auto updateMatch = std::bind(&BracketMatcherView::updateMatch, this);
  this->editor = editor;
  this->matchManager = matchManager;
  this->pairHighlighted = false;
  this->tagHighlighted = false;

  //this->editor->onDidTokenize(this.updateMatch);
  this->editor->getBuffer()->onDidChangeText(updateMatch);
  //this->editor->onDidChangeGrammar(updateMatch);
  this->editor->onDidChangeSelectionRange(updateMatch);
  this->editor->onDidAddCursor(updateMatch);
  this->editor->onDidRemoveCursor(updateMatch);

  this->updateMatch();
}

BracketMatcherView::~BracketMatcherView() {}

void BracketMatcherView::updateMatch() {
  if (this->pairHighlighted) {
    this->editor->destroyMarker(this->startMarker->id);
    this->editor->destroyMarker(this->endMarker->id);
  }

  this->pairHighlighted = false;
  this->tagHighlighted = false;

  if (!this->editor->getLastSelection()->isEmpty()) return;

  //const auto [position, matchPosition] = this->findCurrentPair();
  const auto pair = this->findCurrentPair();
  const auto &position = std::get<0>(pair);
  const auto &matchPosition = std::get<1>(pair);

  Range startRange;
  Range endRange;
  bool highlightTag = false;
  bool highlightPair = false;
  if (position && matchPosition) {
    Range bracket1Range = (startRange = Range(*position, position->traverse(ONE_CHAR_FORWARD_TRAVERSAL)));
    Range bracket2Range = (endRange = Range(*matchPosition, matchPosition->traverse(ONE_CHAR_FORWARD_TRAVERSAL)));
    highlightPair = true;
  } else {
    /*this.bracket1Range = null;
    this.bracket2Range = null;
    if (this.hasSyntaxTree()) {
      ({startRange, endRange} = this.findMatchingTagNameRangesWithSyntaxTree());
    } else {
      ({startRange, endRange} = this.tagFinder.findMatchingTags());
      if (this.isCursorOnCommentOrString()) return;
    }
    if (startRange) {
      highlightTag = true;
      highlightPair = true;
    }*/
  }

  if (!highlightTag && !highlightPair) return;

  this->startMarker = this->createMarker(startRange);
  this->endMarker = this->createMarker(endRange);
  this->pairHighlighted = highlightPair;
  this->tagHighlighted = highlightTag;
}

optional<Point> BracketMatcherView::findMatchingEndBracket(Point startBracketPosition, char16_t startBracket, char16_t endBracket) {
  if (startBracket == endBracket) return optional<Point>();

  if (TreeSitterLanguageMode *languageMode = this->hasSyntaxTree()) {
    return this->findMatchingEndBracketWithSyntaxTree(startBracketPosition, startBracket, endBracket, languageMode);
  } else {
    return optional<Point>();
  }
}

optional<Point> BracketMatcherView::findMatchingStartBracket(Point endBracketPosition, char16_t startBracket, char16_t endBracket) {
  if (startBracket == endBracket) return optional<Point>();

  if (TreeSitterLanguageMode *languageMode = this->hasSyntaxTree()) {
    return this->findMatchingStartBracketWithSyntaxTree(endBracketPosition, startBracket, endBracket, languageMode);
  } else {
    return optional<Point>();
  }
}

static Point PointToJS(TSPoint point) {
  return Point(point.row, point.column / 2);
}
static Point startPosition(TSNode node) {
  return PointToJS(ts_node_start_point(node));
}
static Point endPosition(TSNode node) {
  return PointToJS(ts_node_end_point(node));
}
/*static Point startPosition(TSRange range) {
  return PointToJS(range.start_point);
}
static Point endPosition(TSRange range) {
  return PointToJS(range.end_point);
}*/
static std::vector<TSNode> children(TSNode node) {
  static TSTreeCursor scratch_cursor = {nullptr, nullptr, {0, 0}};
  std::vector<TSNode> result;
  ts_tree_cursor_reset(&scratch_cursor, node);
  if (ts_tree_cursor_goto_first_child(&scratch_cursor)) {
    do {
      TSNode child = ts_tree_cursor_current_node(&scratch_cursor);
      result.push_back(child);
    } while (ts_tree_cursor_goto_next_sibling(&scratch_cursor));
  }
  return result;
}

optional<Point> BracketMatcherView::findMatchingEndBracketWithSyntaxTree(Point bracketPosition, char16_t startBracket, char16_t endBracket, TreeSitterLanguageMode *languageMode) {
  optional<Point> result;
  const Point bracketEndPosition = bracketPosition.traverse({0, 1});
  languageMode->getSyntaxNodeContainingRange(
    Range(bracketPosition, bracketEndPosition),
    [&](TSNode node, TreeSitterGrammar *) {
      if (bracketEndPosition.isGreaterThan(startPosition(node)) && bracketEndPosition.isLessThan(endPosition(node))) {
        auto children = ::children(node);
        auto matchNode = std::find_if(children.begin(), children.end(), [&](TSNode child) {
          return bracketEndPosition.isLessThanOrEqual(startPosition(child)) && ts_node_type(child)[0] == endBracket;
        });
        if (matchNode != children.end()) result = startPosition(*matchNode);
        return true;
      }
      return false;
    }
  );
  return result;
}

optional<Point> BracketMatcherView::findMatchingStartBracketWithSyntaxTree(Point bracketPosition, char16_t startBracket, char16_t endBracket, TreeSitterLanguageMode *languageMode) {
  optional<Point> result;
  const Point bracketEndPosition = bracketPosition.traverse({0, 1});
  languageMode->getSyntaxNodeContainingRange(
    Range(bracketPosition, bracketEndPosition),
    [&](TSNode node, TreeSitterGrammar *) {
      if (bracketPosition.isGreaterThan(startPosition(node))) {
        auto children = ::children(node);
        auto matchNode = std::find_if(children.begin(), children.end(), [&](TSNode child) {
          return bracketPosition.isGreaterThanOrEqual(endPosition(child)) && ts_node_type(child)[0] == startBracket;
        });
        if (matchNode != children.end()) result = startPosition(*matchNode);
        return true;
      }
      return false;
    }
  );
  return result;
}

DisplayMarker *BracketMatcherView::createMarker(Range bufferRange) {
  DisplayMarker *marker = this->editor->markBufferRange(bufferRange);
  this->editor->decorateMarker(marker, {Decoration::Type::highlight, "bracket-matcher"});
  return marker;
}

std::tuple<optional<Point>, optional<Point>> BracketMatcherView::findCurrentPair() {
  const Point currentPosition = this->editor->getCursorBufferPosition();
  const Point previousPosition = currentPosition.traverse(ONE_CHAR_BACKWARD_TRAVERSAL);
  const Point nextPosition = currentPosition.traverse(ONE_CHAR_FORWARD_TRAVERSAL);
  const std::u16string currentCharacter = this->editor->getTextInBufferRange(Range(currentPosition, nextPosition));
  const std::u16string previousCharacter = this->editor->getTextInBufferRange(Range(previousPosition, currentPosition));

  optional<Point> position;
  optional<Point> matchPosition;
  char16_t currentBracket;
  char16_t matchingBracket;
  if (currentCharacter.size() == 1 && this->matchManager->pairedCharacters.count(currentCharacter[0])) {
    matchingBracket = this->matchManager->pairedCharacters[currentCharacter[0]];
    position = currentPosition;
    currentBracket = currentCharacter[0];
    matchPosition = this->findMatchingEndBracket(*position, currentBracket, matchingBracket);
  } else if (previousCharacter.size() == 1 && this->matchManager->pairedCharacters.count(previousCharacter[0])) {
    matchingBracket = this->matchManager->pairedCharacters[previousCharacter[0]];
    position = previousPosition;
    currentBracket = previousCharacter[0];
    matchPosition = this->findMatchingEndBracket(*position, currentBracket, matchingBracket);
  } else if (previousCharacter.size() == 1 && this->matchManager->pairedCharactersInverse.count(previousCharacter[0])) {
    matchingBracket = this->matchManager->pairedCharactersInverse[previousCharacter[0]];
    position = previousPosition;
    currentBracket = previousCharacter[0];
    matchPosition = this->findMatchingStartBracket(*position, matchingBracket, currentBracket);
  } else if (currentCharacter.size() == 1 && this->matchManager->pairedCharactersInverse.count(currentCharacter[0])) {
    matchingBracket = this->matchManager->pairedCharactersInverse[currentCharacter[0]];
    position = currentPosition;
    currentBracket = currentCharacter[0];
    matchPosition = this->findMatchingStartBracket(*position, matchingBracket, currentBracket);
  }

  return {position, matchPosition};
}

TreeSitterLanguageMode *BracketMatcherView::hasSyntaxTree() {
  return dynamic_cast<TreeSitterLanguageMode *>(this->editor->buffer->getLanguageMode());
}