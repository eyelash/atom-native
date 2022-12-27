#ifndef TEXT_EDITOR_H_
#define TEXT_EDITOR_H_

#include "decoration.h"
#include <vector>
#include <functional>
#include <range.h>
#include <optional.h>
#include <display-layer.h>
#include <text-buffer.h>

class Cursor;
class Selection;
class DisplayMarker;
class DecorationManager;
class LayerDecoration;

class TextEditor {
  std::vector<Cursor *> cursors;
  std::vector<Selection *> selections;
  TextBuffer *buffer;
public:
  DisplayLayer *displayLayer;
  DisplayMarkerLayer *selectionsMarkerLayer;
  DecorationManager *decorationManager;

public:
  TextEditor();
  ~TextEditor();

  void subscribeToBuffer();
  void subscribeToDisplayLayer();
  TextBuffer *getBuffer();
  bool isModified();
  bool isEmpty();
  std::u16string getText();
  std::u16string getTextInBufferRange(Range);
  unsigned getLineCount();
  double getScreenLineCount();
  double getApproximateScreenLineCount();
  unsigned getLastBufferRow();
  double getLastScreenRow();
  optional<std::u16string> lineTextForBufferRow(uint32_t);
  std::u16string lineTextForScreenRow(double);
  DisplayLayer::ScreenLine screenLineForScreenRow(double);
  double bufferRowForScreenRow(double);
  double screenRowForBufferRow(double);
  Point getRightmostScreenPosition();
  double getMaxScreenLineLength();
  double getLongestScreenRow();
  double lineLengthForScreenRow(double);
  Range bufferRangeForBufferRow(double, bool = false);
  std::u16string getTextInRange(Range);
  Point getEofBufferPosition();
  void setText(std::u16string &&);
  Range setTextInBufferRange(Range, std::u16string &&);
  void insertText(const std::u16string &);
  void insertNewline();
  void delete_();
  void backspace();
  void mutateSelectedText(std::function<void(Selection *)>);
  Point screenPositionForBufferPosition(Point);
  Point bufferPositionForScreenPosition(Point);
  Range screenRangeForBufferRange(Range);
  Range bufferRangeForScreenRange(Range);
  Point clipBufferPosition(Point);
  Range clipBufferRange(Range);
  Point clipScreenPosition(Point);
  Range clipScreenRange(Range);
  Decoration *decorateMarker(DisplayMarker *, Decoration::Properties);
  LayerDecoration *decorateMarkerLayer(DisplayMarkerLayer *, Decoration::Properties);
  DisplayMarkerLayer *addMarkerLayer();
  Point getCursorBufferPosition();
  void setCursorBufferPosition(Point);
  Point getCursorScreenPosition();
  void setCursorScreenPosition(Point);
  Cursor *addCursorAtBufferPosition(Point);
  Cursor *addCursorAtScreenPosition(Point);
  bool hasMultipleCursors() const;
  void moveUp(double = 1);
  void moveDown(double = 1);
  void moveLeft(double = 1);
  void moveRight(double = 1);
  void moveToBeginningOfLine();
  void moveToBeginningOfScreenLine();
  void moveToFirstCharacterOfLine();
  void moveToEndOfLine();
  void moveToEndOfScreenLine();
  void moveToBeginningOfWord();
  void moveToEndOfWord();
  void moveToTop();
  void moveToBottom();
  void moveToBeginningOfNextWord();
  void moveToPreviousWordBoundary();
  void moveToNextWordBoundary();
  void moveToPreviousSubwordBoundary();
  void moveToNextSubwordBoundary();
  void moveToBeginningOfNextParagraph();
  void moveToBeginningOfPreviousParagraph();
  Cursor *getLastCursor();
  std::vector<Cursor *> getCursors() const;
  std::vector<Cursor *> getCursorsOrderedByBufferPosition();
  Cursor *addCursor(DisplayMarker *);
  std::u16string getSelectedText();
  Range getSelectedBufferRange();
  Range getSelectedScreenRange();
  Selection *addSelectionForBufferRange(Range);
  Selection *addSelectionForScreenRange(Range);
  Selection *getLastSelection();
  void selectUp(double = 1);
  void selectDown(double = 1);
  void selectLeft(double = 1);
  void selectRight(double = 1);
  void selectToTop();
  void selectToBottom();
  void selectAll();
  void selectToBeginningOfLine();
  void selectToFirstCharacterOfLine();
  void selectToEndOfLine();
  void selectToBeginningOfWord();
  void selectToEndOfWord();
  void selectToPreviousSubwordBoundary();
  void selectToNextSubwordBoundary();
  void selectLinesContainingCursors();
  void selectWordsContainingCursors();
  void selectToPreviousWordBoundary();
  void selectToNextWordBoundary();
  void selectToBeginningOfNextWord();
  void selectToBeginningOfNextParagraph();
  void selectToBeginningOfPreviousParagraph();
  std::vector<Selection *> getSelections();
  std::vector<Selection *> getSelectionsOrderedByBufferPosition();
  void addSelectionBelow();
  void addSelectionAbove();
  void expandSelectionsForward(std::function<void(Selection *)>);
  void expandSelectionsBackward(std::function<void(Selection *)>);
  void mergeIntersectingSelections(std::function<void()>);
  void mergeIntersectingSelections();
  void avoidMergingSelections(std::function<void()>);
  void mergeSelections(std::function<void()>, std::function<bool(Selection *, Selection *)>);
  void addSelection(DisplayMarker *);
  void removeSelection(Selection *);
  bool consolidateSelections();
  void scan(const Regex &, TextBuffer::ScanIterator);
  void scanInBufferRange(const Regex &, Range, TextBuffer::ScanIterator);
  void backwardsScanInBufferRange(const Regex &, Range, TextBuffer::ScanIterator);
  const char16_t *getNonWordCharacters(Point);

private:
  void moveCursors(std::function<void(Cursor *)>);
  void mergeCursors();
  void createLastSelectionIfNeeded();
};

#endif  // TEXT_EDITOR_H_
