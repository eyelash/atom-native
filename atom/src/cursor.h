#ifndef CURSOR_H_
#define CURSOR_H_

#include <display-layer.h>
#include <range.h>
#include <optional.h>
#include <regex.h>
#include <functional>

struct TextEditor;
struct DisplayMarker;
struct Selection;

struct Cursor {
  TextEditor *editor;
  DisplayMarker *marker;
  optional<double> goalColumn;
  Selection *selection;

  Cursor(TextEditor *, DisplayMarker *);
  ~Cursor();
  void destroy();

  void setScreenPosition(const Point &, optional<bool> = {}, DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest);
  void setScreenPosition(const Point &, DisplayLayer::ClipDirection);
  Point getScreenPosition();
  void setBufferPosition(const Point &, optional<bool> = {});
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
  double getIndentLevel();
  bool hasPrecedingCharactersOnLine();
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
  std::u16string getNonWordCharacters();
  void changePosition(optional<bool>, std::function<void()>);
  Range getScreenRange();
  void autoscroll();
  Point getBeginningOfNextParagraphBufferPosition();
  Point getBeginningOfPreviousParagraphBufferPosition();
};

#endif // CURSOR_H_
