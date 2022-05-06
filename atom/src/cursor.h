#ifndef CURSOR_H_
#define CURSOR_H_

#include <point.h>
#include <optional.h>
#include <functional>

class TextEditor;
class DisplayMarker;

class Cursor {
  TextEditor *editor;
  DisplayMarker *marker;

public:
  Cursor(TextEditor *, DisplayMarker *);
  ~Cursor();

  void setScreenPosition(Point, optional<bool> = optional<bool>());
  Point getScreenPosition();
  void setBufferPosition(Point, optional<bool> = optional<bool>());
  Point getBufferPosition();
  double getScreenRow();
  double getScreenColumn();
  double getBufferRow();
  double getBufferColumn();
  bool isAtBeginningOfLine();
  DisplayMarker *getMarker();
  void moveUp(double = 1, bool = false);
  void moveDown(double = 1, bool = false);
  void moveLeft(double = 1, bool = false);
  void moveRight(double = 1, bool = false);
  void clearSelection(bool);

private:
  void changePosition(optional<bool>, std::function<void()>);
};

#endif  // CURSOR_H_
