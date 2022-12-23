#ifndef CURSOR_H_
#define CURSOR_H_

#include <range.h>
#include <optional.h>
#include <functional>

class TextEditor;
class DisplayMarker;
class Selection;

class Cursor {
  TextEditor *editor;
  DisplayMarker *marker;
  optional<double> goalColumn;

public:
  Selection *selection;

  Cursor(TextEditor *, DisplayMarker *);
  ~Cursor();
  void destroy();

  void setScreenPosition(Point, optional<bool> = optional<bool>());
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
  bool isLastCursor();
  void moveUp(double = 1, bool = false);
  void moveDown(double = 1, bool = false);
  void moveLeft(double = 1, bool = false);
  void moveRight(double = 1, bool = false);
  void moveToTop();
  void moveToBottom();
  void moveToBeginningOfScreenLine();
  void moveToBeginningOfLine();
  void moveToEndOfScreenLine();
  void moveToEndOfLine();
  Range getCurrentLineBufferRange(bool = false);
  int compare(Cursor *);
  void clearSelection(bool);

private:
  void changePosition(optional<bool>, std::function<void()>);
};

#endif  // CURSOR_H_
