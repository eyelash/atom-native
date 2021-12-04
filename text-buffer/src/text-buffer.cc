#include "text-buffer.h"

TextBuffer::TextBuffer() :
  buffer{new NativeTextBuffer()} {}

TextBuffer::TextBuffer(const std::u16string &text) :
  buffer{new NativeTextBuffer(text)} {}

TextBuffer::~TextBuffer() {
  delete buffer;
}

std::u16string TextBuffer::getText() {
  return buffer->text();
}
