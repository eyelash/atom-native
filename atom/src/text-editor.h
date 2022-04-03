#ifndef TEXT_EDITOR_H_
#define TEXT_EDITOR_H_

#include <range.h>
#include <optional.h>

class TextBuffer;

class TextEditor {
  TextBuffer *buffer;

public:
  TextEditor();
  ~TextEditor();

  TextBuffer *getBuffer();
  std::u16string getText();
  std::u16string getTextInBufferRange(Range);
  unsigned getLineCount();
  unsigned getLastBufferRow();
  optional<std::u16string> lineTextForBufferRow(uint32_t bufferRow);
  void setText(std::u16string &&);
  Range setTextInBufferRange(Range, std::u16string &&);
};

#endif  // TEXT_EDITOR_H_
