#ifndef TEXT_EDITOR_H_
#define TEXT_EDITOR_H_

#include <range.h>
#include <optional.h>

class TextBuffer;
class DisplayLayer;

class TextEditor {
  TextBuffer *buffer;
  DisplayLayer *displayLayer;

public:
  TextEditor();
  ~TextEditor();

  TextBuffer *getBuffer();
  std::u16string getText();
  std::u16string getTextInBufferRange(Range);
  unsigned getLineCount();
  double getScreenLineCount();
  unsigned getLastBufferRow();
  double getLastScreenRow();
  optional<std::u16string> lineTextForBufferRow(uint32_t);
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
};

#endif  // TEXT_EDITOR_H_
