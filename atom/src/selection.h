#ifndef SELECTION_H_
#define SELECTION_H_

#include <range.h>

class Cursor;
class DisplayMarker;
class TextEditor;

class Selection {
  TextEditor *editor;
  DisplayMarker *marker;

public:
  Cursor *cursor;

  Selection(TextEditor *, DisplayMarker *, Cursor *);
  ~Selection();

  Range getScreenRange();
  Range getBufferRange();
  void insertText(const std::u16string &);
  int compare(Selection *);
};

#endif  // SELECTION_H_
