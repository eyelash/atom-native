#ifndef TEXT_EDITOR_H_
#define TEXT_EDITOR_H_

#include "decoration.h"
#include <vector>
#include <functional>
#include <range.h>
#include <optional.h>
#include <display-layer.h>
#include <text-buffer.h>
#include <event-kit.h>

class Cursor;
class Selection;
class DisplayMarker;
class DecorationManager;
class LayerDecoration;

class TextEditor {
  bool softTabs;
  double undoGroupingInterval;
  bool suppressSelectionMerging;
  Emitter<> didChangeTitleEmitter;
  Emitter<> didChangeEmitter;
  Emitter<Range> didRequestAutoscrollEmitter;
  std::vector<Cursor *> cursors;
  std::unordered_map<unsigned, Cursor *> cursorsByMarkerId;
  std::vector<Selection *> selections;
  TextBuffer *buffer;
public:
  DisplayLayer *displayLayer;
  DisplayMarkerLayer *selectionsMarkerLayer;
  DecorationManager *decorationManager;

public:
  TextEditor(TextBuffer * = nullptr);
  ~TextEditor();

  void decorateCursorLine();
  void subscribeToBuffer();
  void subscribeToDisplayLayer();
  void onDidChangeTitle(std::function<void()>);
  void onDidChange(std::function<void()>);
  void onDidChangeModified(std::function<void()>);
  void onDidRequestAutoscroll(std::function<void(Range)>);
  TextBuffer *getBuffer();
  std::string getTitle();
  optional<std::string> getPath();
  optional<std::string> getFileName();
  bool isModified();
  bool isEmpty();
  TextBuffer *save();
  TextBuffer *saveAs(const std::string &);
  std::u16string getText();
  std::u16string getTextInBufferRange(Range);
  double getLineCount();
  double getScreenLineCount();
  double getApproximateScreenLineCount();
  double getLastBufferRow();
  double getLastScreenRow();
  optional<std::u16string> lineTextForBufferRow(double);
  std::u16string lineTextForScreenRow(double);
  DisplayLayer::ScreenLine screenLineForScreenRow(double);
  double bufferRowForScreenRow(double);
  std::vector<double> bufferRowsForScreenRows(double, double);
  double screenRowForBufferRow(double);
  Point getRightmostScreenPosition();
  double getMaxScreenLineLength();
  double getLongestScreenRow();
  double lineLengthForScreenRow(double);
  Range bufferRangeForBufferRow(double, bool = false);
  std::u16string getTextInRange(Range);
  bool isBufferRowBlank(double);
  optional<double> nextNonBlankBufferRow(double);
  Point getEofBufferPosition();
  void setText(std::u16string &&);
  Range setTextInBufferRange(Range, std::u16string &&);
  void insertText(const std::u16string &, bool = false);
  void insertNewline();
  void delete_();
  void backspace();
  void mutateSelectedText(std::function<void(Selection *)>, double = 0);
  void moveLineUp();
  void moveLineDown();
  void duplicateLines();
  void replaceSelectedText(bool, std::function<std::u16string(std::u16string)>);
  void splitSelectionsIntoLines();
  void upperCase();
  void lowerCase();
  void insertNewlineBelow();
  void insertNewlineAbove();
  void deleteToBeginningOfWord();
  void deleteToPreviousWordBoundary();
  void deleteToNextWordBoundary();
  void deleteToBeginningOfSubword();
  void deleteToEndOfSubword();
  void deleteToBeginningOfLine();
  void deleteToEndOfLine();
  void deleteToEndOfWord();
  void deleteLine();
  void undo();
  void redo();
  void transact(double, std::function<void()>);
  void transact(std::function<void()>);
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
  Cursor *addCursorAtBufferPosition(Point, bool = true);
  Cursor *addCursorAtScreenPosition(Point, bool = true);
  bool hasMultipleCursors();
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
  std::vector<Cursor *> getCursors();
  std::vector<Cursor *> getCursorsOrderedByBufferPosition();
  Cursor *addCursor(DisplayMarker *);
  std::u16string getSelectedText();
  Range getSelectedBufferRange();
  std::vector<Range> getSelectedBufferRanges();
  void setSelectedBufferRange(Range, bool = true);
  void setSelectedBufferRanges(const std::vector<Range> &, bool = true);
  Range getSelectedScreenRange();
  Selection *addSelectionForBufferRange(Range, bool = true);
  Selection *addSelectionForScreenRange(Range, bool = true);
  void selectToBufferPosition(Point);
  void selectToScreenPosition(Point, bool = false);
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
  Selection *getLastSelection();
  Selection *getSelectionAtScreenPosition(Point);
  std::vector<Selection *> getSelections();
  std::vector<Selection *> getSelectionsOrderedByBufferPosition();
  void addSelectionBelow();
  void addSelectionAbove();
  void expandSelectionsForward(std::function<void(Selection *)>);
  void expandSelectionsBackward(std::function<void(Selection *)>);
  void finalizeSelections();
  void mergeIntersectingSelections(std::function<void()>);
  void mergeIntersectingSelections();
  void mergeSelectionsOnSameRows(std::function<void()>);
  void mergeSelectionsOnSameRows();
  void avoidMergingSelections(std::function<void()>);
  void mergeSelections(std::function<void()>, std::function<bool(Selection *, Selection *)>);
  void addSelection(DisplayMarker *);
  void removeSelection(Selection *);
  bool consolidateSelections();
  void scan(const Regex &, TextBuffer::ScanIterator);
  void scanInBufferRange(const Regex &, Range, TextBuffer::ScanIterator);
  void backwardsScanInBufferRange(const Regex &, Range, TextBuffer::ScanIterator);
  bool getSoftTabs();
  double getTabLength();
  std::u16string getTabText();
  double indentationForBufferRow(double bufferRow);
  Range setIndentationForBufferRow(double, double, bool = false);
  void indentSelectedRows();
  void outdentSelectedRows();
  double indentLevelForLine(const std::u16string &);
  void indent();
  std::u16string buildIndentString(double, double = 0);
  void scrollToCursorPosition();
  void scrollToBufferPosition(Point);
  void scrollToScreenPosition(Point);
  void scrollToScreenRange(Range, bool = true);
  bool shouldAutoIndent();
  double getUndoGroupingInterval();
  const char16_t *getNonWordCharacters(Point);
  double suggestedIndentForBufferRow(double bufferRow, bool);
  Range autoIndentBufferRow(double bufferRow, bool = false, bool = true);
  void autoIndentBufferRows(double startRow, double endRow);
  void autoDecreaseIndentForBufferRow(double bufferRow);

private:
  void moveCursors(std::function<void(Cursor *)>);
  void mergeCursors();
  void createLastSelectionIfNeeded();
};

#endif // TEXT_EDITOR_H_
