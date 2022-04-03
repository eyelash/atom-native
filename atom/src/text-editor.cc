#include "text-editor.h"
#include <text-buffer.h>

TextEditor::TextEditor() :
  buffer{new TextBuffer()} {}

TextEditor::~TextEditor() {
  delete buffer;
}

/*
Section: Event Subscription
*/

/*
Section: Buffer
*/

TextBuffer *TextEditor::getBuffer() {
  return this->buffer;
}

/*
Section: File Details
*/

/*
Section: File Operations
*/

/*
Section: Reading Text
*/

std::u16string TextEditor::getText() {
  return this->buffer->getText();
}

std::u16string TextEditor::getTextInBufferRange(Range range) {
  return this->buffer->getTextInRange(range);
}

unsigned TextEditor::getLineCount() {
  return this->buffer->getLineCount();
}

unsigned TextEditor::getLastBufferRow() {
  return this->buffer->getLastRow();
}

optional<std::u16string> TextEditor::lineTextForBufferRow(uint32_t bufferRow) {
  return this->buffer->lineForRow(bufferRow);
}

/*
Section: Mutating Text
*/

void TextEditor::setText(std::u16string &&text) {
  this->buffer->setText(std::move(text));
}

Range TextEditor::setTextInBufferRange(Range range, std::u16string &&text) {
  return this->getBuffer()->setTextInRange(range, std::move(text));
}
