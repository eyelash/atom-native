#ifndef CURSOR_H_
#define CURSOR_H_

#include <display-layer.h>
#include <range.h>
#include <optional.h>
#include <regex.h>
#include <functional>

class TextEditor;
class DisplayMarker;
class Selection;

class Cursor {
public:
  TextEditor *editor;
  DisplayMarker *marker;
  optional<double> goalColumn;
  Selection *selection;

  Cursor(TextEditor *, DisplayMarker *);
  ~Cursor();
  void destroy();

  void setScreenPosition(Point, optional<bool> = optional<bool>(), DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest);
  void setScreenPosition(Point, DisplayLayer::ClipDirection);
  Point getScreenPosition();
  void setBufferPosition(Point, optional<bool> = optional<bool>());
  Point getBufferPosition();
  double getScreenRow();
  double getScreenColumn();
  double getBufferRow();
  double getBufferColumn();
  std::u16string getCurrentBufferLine();
  bool isAtBeginningOfLine();
  bool isAtEndOfLine();
  DisplayMarker *getMarker();
  bool isSurroundedByWhitespace();
  bool isBetweenWordAndNonWord();
  bool isInsideWord();
  bool isLastCursor();
  void moveUp(double = 1, bool = false);
  void moveDown(double = 1, bool = false);
  void moveLeft(double = 1, bool = false);
  void moveRight(double = 1, bool = false);
  void moveToTop();
  void moveToBottom();
  void moveToBeginningOfScreenLine();
  void moveToBeginningOfLine();
  void moveToFirstCharacterOfLine();
  void moveToEndOfScreenLine();
  void moveToEndOfLine();
  void moveToBeginningOfWord();
  void moveToEndOfWord();
  void moveToBeginningOfNextWord();
  void moveToPreviousWordBoundary();
  void moveToNextWordBoundary();
  void moveToPreviousSubwordBoundary();
  void moveToNextSubwordBoundary();
  void skipLeadingWhitespace();
  void moveToBeginningOfNextParagraph();
  void moveToBeginningOfPreviousParagraph();
  Point getPreviousWordBoundaryBufferPosition(const Regex * = nullptr);
  Point getNextWordBoundaryBufferPosition(const Regex * = nullptr);
  Point getBeginningOfCurrentWordBufferPosition();
  Point getEndOfCurrentWordBufferPosition();
  Point getBeginningOfNextWordBufferPosition();
  Range getCurrentWordBufferRange(const Regex * = nullptr, bool = true);
  Range getCurrentLineBufferRange(bool = false);
  int compare(Cursor *);
  void clearSelection(bool);
  Regex wordRegExp(bool = true);
  Regex subwordRegExp(bool = false);
  const char16_t *getNonWordCharacters();
  void changePosition(optional<bool>, std::function<void()>);
  Range getScreenRange();
  void autoscroll();
  Point getBeginningOfNextParagraphBufferPosition();
  Point getBeginningOfPreviousParagraphBufferPosition();
};

#endif // CURSOR_H_
