#ifndef SELECTION_H_
#define SELECTION_H_

#include <range.h>
#include <optional.h>
#include <event-kit.h>
#include <functional>

struct Cursor;
struct DisplayMarker;
struct TextEditor;

struct Selection {
  Cursor *cursor;
  DisplayMarker *marker;
  TextEditor *editor;
  Emitter<> didChangeRangeEmitter;
  //Emitter<> didDestroyEmitter;
  optional<Range> initialScreenRange;
  optional<Range> goalScreenRange;
  bool wordwise;
  bool linewise;
  bool retainSelection;
  std::vector<Range> rangeStack_;

  Selection(TextEditor *, DisplayMarker *, Cursor *);
  ~Selection();
  void destroy();

  bool isLastSelection();
  void onDidChangeRange(std::function<void()>);
  void onDidDestroy(std::function<void()>);
  Range getScreenRange();
  void setScreenRange(Range, optional<bool> = {});
  Range getBufferRange();
  void setBufferRange(Range, optional<bool> = {}, optional<bool> = {});
  std::pair<double, double> getBufferRowRange();
  Point getTailScreenPosition();
  Point getTailBufferPosition();
  Point getHeadScreenPosition();
  Point getHeadBufferPosition();
  bool isEmpty();
  bool isReversed();
  bool isSingleScreenLine();
  std::u16string getText();
  bool intersectsBufferRange(Range);
  bool intersectsScreenRowRange(double, double);
  bool intersectsScreenRow(double);
  bool intersectsWith(Selection *, bool);
  void clear(optional<bool> = {});
  void selectToScreenPosition(Point position);
  void selectToBufferPosition(Point);
  void selectRight(double = 1);
  void selectLeft(double = 1);
  void selectUp(double = 1);
  void selectDown(double = 1);
  void selectToTop();
  void selectToBottom();
  void selectAll();
  void selectToBeginningOfLine();
  void selectToFirstCharacterOfLine();
  void selectToEndOfLine();
  void selectToEndOfBufferLine();
  void selectToBeginningOfWord();
  void selectToEndOfWord();
  void selectToBeginningOfNextWord();
  void selectToPreviousWordBoundary();
  void selectToNextWordBoundary();
  void selectToPreviousSubwordBoundary();
  void selectToNextSubwordBoundary();
  void selectToBeginningOfNextParagraph();
  void selectToBeginningOfPreviousParagraph();
  void selectWord();
  void expandOverWord(optional<bool> = {});
  void selectLine(optional<double> = {});
  void expandOverLine(optional<bool> = {});
  Range insertText(const std::u16string &);
  void backspace();
  void deleteToPreviousWordBoundary();
  void deleteToNextWordBoundary();
  void deleteToBeginningOfWord();
  void deleteToBeginningOfLine();
  void delete_();
  void deleteToEndOfLine();
  void deleteToEndOfWord();
  void deleteToBeginningOfSubword();
  void deleteToEndOfSubword();
  void deleteSelectedText();
  void deleteLine();
  void outdentSelectedRows();
  void cut(std::u16string &, bool = false, bool = false);
  void copy(std::u16string &, bool = false, bool = false);
  void indent(bool = false);
  void indentSelectedRows();
  void addSelectionBelow();
  void addSelectionAbove();
  void merge(Selection *);
  int compare(Selection *);
  void setGoalScreenRange(Range);
  Range getGoalScreenRange();
  void markerDidChange();
  void markerDidDestroy();
  void finalize();
  void autoscroll();
  void modifySelection(std::function<void()>);
  void plantTail();
};

#endif // SELECTION_H_
