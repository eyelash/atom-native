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

struct Cursor;
struct Selection;
struct DisplayMarker;
struct DecorationManager;
struct LayerDecoration;
struct Clipboard;
struct Grammar;

struct TextEditor {
  static Clipboard clipboard;

  bool softTabs;
  double undoGroupingInterval;
  bool suppressSelectionMerging;
  Emitter<> didChangeTitleEmitter;
  Emitter<> didChangePathEmitter;
  Emitter<> didChangeEmitter;
  Emitter<> didChangeSelectionRangeEmitter;
  Emitter<const std::u16string &, const Range &> didInsertTextEmitter;
  Emitter<> didAddCursorEmitter;
  Emitter<> didRemoveCursorEmitter;
  Emitter<const Range &> didRequestAutoscrollEmitter;
  std::vector<Cursor *> cursors;
  std::unordered_map<unsigned, Cursor *> cursorsByMarkerId;
  std::vector<Selection *> selections;
  TextBuffer *buffer;
  DisplayLayer *displayLayer;
  DisplayMarkerLayer *defaultMarkerLayer;
  DisplayMarkerLayer *selectionsMarkerLayer;
  DecorationManager *decorationManager;

  TextEditor(TextBuffer * = nullptr);
  ~TextEditor();

  void decorateCursorLine();
  void finishUpdate(const DisplayLayer::Params & = {});
  void updateSoftTabs(bool, bool);
  void updateAtomicSoftTabs(bool, bool, DisplayLayer::Params &);
  void updateAtomicSoftTabs(bool, bool);
  void updateTabLength(double, bool, DisplayLayer::Params &);
  void updateTabLength(double, bool);
  void subscribeToBuffer();
  void subscribeToDisplayLayer();
  void onDidChangeTitle(std::function<void()>);
  void onDidChangePath(std::function<void()>);
  void onDidChange(std::function<void()>);
  void onDidChangeSelectionRange(std::function<void()>);
  void onDidChangeGrammar(std::function<void()>);
  void onDidChangeModified(std::function<void()>);
  void onDidInsertText(std::function<void(const std::u16string &, const Range &)>);
  void onDidAddCursor(std::function<void()>);
  void onDidRemoveCursor(std::function<void()>);
  void onDidRequestAutoscroll(std::function<void(const Range &)>);
  TextBuffer *getBuffer();
  std::string getTitle();
  optional<std::string> getPath();
  optional<std::string> getFileName();
  bool isModified();
  bool isEmpty();
  TextBuffer *save();
  TextBuffer *saveAs(const std::string &);
  std::u16string getText();
  std::u16string getTextInBufferRange(const Range &);
  double getLineCount();
  double getScreenLineCount();
  double getApproximateScreenLineCount();
  double getLastBufferRow();
  double getLastScreenRow();
  std::u16string lineTextForBufferRow(double);
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
  std::u16string getTextInRange(const Range &);
  bool isBufferRowBlank(double);
  optional<double> nextNonBlankBufferRow(double);
  Point getEofBufferPosition();
  void setText(const std::u16string &);
  Range setTextInBufferRange(const Range &, const std::u16string &);
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
  void transpose();
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
  Point screenPositionForBufferPosition(const Point &);
  Point bufferPositionForScreenPosition(const Point &);
  Range screenRangeForBufferRange(const Range &);
  Range bufferRangeForScreenRange(const Range &);
  Point clipBufferPosition(const Point &);
  Range clipBufferRange(const Range &);
  Point clipScreenPosition(const Point &);
  Range clipScreenRange(const Range &);
  Decoration *decorateMarker(DisplayMarker *, Decoration::Properties);
  LayerDecoration *decorateMarkerLayer(DisplayMarkerLayer *, Decoration::Properties);
  DisplayMarker *markBufferRange(const Range &);
  DisplayMarker *markScreenRange(const Range &);
  DisplayMarker *markBufferPosition(const Point &);
  DisplayMarker *markScreenPosition(const Point &);
  DisplayMarker *getMarker(unsigned id);
  size_t getMarkerCount();
  void destroyMarker(unsigned id);
  DisplayMarkerLayer *addMarkerLayer(bool = false);
  DisplayMarkerLayer *getMarkerLayer(unsigned);
  DisplayMarkerLayer *getDefaultMarkerLayer();
  Point getCursorBufferPosition();
  void setCursorBufferPosition(const Point &);
  Cursor *getCursorAtScreenPosition(const Point &);
  Point getCursorScreenPosition();
  void setCursorScreenPosition(const Point &);
  Cursor *addCursorAtBufferPosition(const Point &, bool = true);
  Cursor *addCursorAtScreenPosition(const Point &, bool = true);
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
  void moveCursors(std::function<void(Cursor *)>);
  void mergeCursors();
  std::u16string getSelectedText();
  Range getSelectedBufferRange();
  std::vector<Range> getSelectedBufferRanges();
  void setSelectedBufferRange(const Range &, bool = true);
  void setSelectedBufferRanges(const std::vector<Range> &, bool = true);
  Range getSelectedScreenRange();
  Selection *addSelectionForBufferRange(const Range &, bool = true);
  Selection *addSelectionForScreenRange(const Range &, bool = true);
  void selectToBufferPosition(const Point &);
  void selectToScreenPosition(const Point &, bool = false);
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
  void selectLargerSyntaxNode();
  void selectSmallerSyntaxNode();
  Selection *getLastSelection();
  Selection *getSelectionAtScreenPosition(const Point &);
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
  void selectionRangeChanged();
  void createLastSelectionIfNeeded();
  void scan(const Regex &, TextBuffer::ScanIterator);
  void scanInBufferRange(const Regex &, const Range &, TextBuffer::ScanIterator);
  void backwardsScanInBufferRange(const Regex &, const Range &, TextBuffer::ScanIterator);
  bool getSoftTabs();
  void setSoftTabs(bool);
  bool hasAtomicSoftTabs();
  void toggleSoftTabs();
  double getTabLength();
  void setTabLength(double);
  optional<bool> usesSoftTabs();
  std::u16string getTabText();
  double indentationForBufferRow(double bufferRow);
  Range setIndentationForBufferRow(double, double, bool = false);
  void indentSelectedRows();
  void outdentSelectedRows();
  double indentLevelForLine(const std::u16string &);
  void autoIndentSelectedRows();
  void indent();
  std::u16string buildIndentString(double, double = 0);
  Grammar *getGrammar();
  void copySelectedText();
  void cutSelectedText();
  void pasteText();
  void scrollToCursorPosition();
  void scrollToBufferPosition(const Point &);
  void scrollToScreenPosition(const Point &);
  void scrollToScreenRange(Range, bool = true);
  bool shouldAutoIndent();
  double getUndoGroupingInterval();
  std::u16string getNonWordCharacters(const Point &);
  double suggestedIndentForBufferRow(double bufferRow, bool = true);
  Range autoIndentBufferRow(double bufferRow, bool = false, bool = true);
  void autoIndentBufferRows(double startRow, double endRow);
  void autoDecreaseIndentForBufferRow(double bufferRow);
};

#endif // TEXT_EDITOR_H_
