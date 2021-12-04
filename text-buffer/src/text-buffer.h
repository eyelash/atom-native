#ifndef TEXT_BUFFER_H_
#define TEXT_BUFFER_H_

#include <string>
#include <native-text-buffer.h>

class TextBuffer {
  NativeTextBuffer *buffer;

public:

  TextBuffer();
  TextBuffer(const std::u16string &text);
  ~TextBuffer();

  std::u16string getText();
};

#endif  // TEXT_BUFFER_H_
