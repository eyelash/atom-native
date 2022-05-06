#ifndef TEXT_EDITOR_H_
#define TEXT_EDITOR_H_

#include <vector>
#include <functional>
#include <range.h>
#include <optional.h>
#include <display-layer.h>

class Cursor;
class Selection;
class TextBuffer;
class DisplayMarker;

class TextEditor {
  std::vector<Cursor *> cursors;
  std::vector<Selection *> selections;
  TextBuffer *buffer;
  DisplayLayer *displayLayer;
  DisplayMarkerLayer *selectionsMarkerLayer;

public:
  TextEditor();
  ~TextEditor();

  void subscribeToBuffer();
  void subscribeToDisplayLayer();
  TextBuffer *getBuffer();
  std::u16string getText();
  std::u16string getTextInBufferRange(Range);
  unsigned getLineCount();
  double getScreenLineCount();
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
  Range bufferRangeForBufferRow(double, bool);
  std::u16string getTextInRange(Range);
  Point getEofBufferPosition();
  void setText(std::u16string &&);
  Range setTextInBufferRange(Range, std::u16string &&);
  void insertText(const std::u16string &);
  void mutateSelectedText(std::function<void(Selection *)>);
  DisplayMarkerLayer *addMarkerLayer();
  void setCursorScreenPosition(Point);
  Cursor *addCursorAtBufferPosition(Point);
  Cursor *addCursorAtScreenPosition(Point);
  bool hasMultipleCursors() const;
  void moveUp(double = 1);
  void moveDown(double = 1);
  void moveLeft(double = 1);
  void moveRight(double = 1);
  std::vector<Cursor *> getCursors() const;
  Cursor *addCursor(DisplayMarker *);
  Selection *addSelectionForBufferRange(Range);
  Selection *getLastSelection();
  std::vector<Selection *> getSelections();
  std::vector<Selection *> getSelectionsOrderedByBufferPosition();
  void addSelection(DisplayMarker *);

private:
  void moveCursors(std::function<void(Cursor *)>);
  void mergeCursors();
  void createLastSelectionIfNeeded();
};

#endif  // TEXT_EDITOR_H_
