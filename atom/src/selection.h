#ifndef SELECTION_H_
#define SELECTION_H_

#include <range.h>
#include <optional.h>
#include <functional>

class Cursor;
class DisplayMarker;
class TextEditor;

class Selection {
  TextEditor *editor;
  DisplayMarker *marker;
  optional<Range> goalScreenRange;
  bool retainSelection;

public:
  Cursor *cursor;

  Selection(TextEditor *, DisplayMarker *, Cursor *);
  ~Selection();
  void destroy();

  bool isLastSelection();
  Range getScreenRange();
  void setScreenRange(Range);
  Range getBufferRange();
  void setBufferRange(Range);
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
  void clear();
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
  void insertText(const std::u16string &);
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
  void merge(Selection *);
  void addSelectionBelow();
  void addSelectionAbove();
  int compare(Selection *);
  void setGoalScreenRange(Range);
  Range getGoalScreenRange();
  void markerDidDestroy();
  void modifySelection(std::function<void()>);
  void plantTail();
};

#endif  // SELECTION_H_
