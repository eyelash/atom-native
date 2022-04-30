#include "text-editor.h"
#include <text-buffer.h>
#include <display-layer.h>

TextEditor::TextEditor() :
  buffer{new TextBuffer()},
  displayLayer{buffer->addDisplayLayer()} {}

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

double TextEditor::getScreenLineCount() {
  return this->displayLayer->getScreenLineCount();
}

/*getApproximateScreenLineCount() {
  return this.displayLayer.getApproximateScreenLineCount();
}*/

unsigned TextEditor::getLastBufferRow() {
  return this->buffer->getLastRow();
}

double TextEditor::getLastScreenRow() {
  return this->getScreenLineCount() - 1;
}

optional<std::u16string> TextEditor::lineTextForBufferRow(uint32_t bufferRow) {
  return this->buffer->lineForRow(bufferRow);
}

/*lineTextForScreenRow(screenRow) {
  const screenLine = this.screenLineForScreenRow(screenRow);
  if (screenLine) return screenLine.lineText;
}

screenLineForScreenRow(screenRow) {
  return this.displayLayer.getScreenLine(screenRow);
}*/

double TextEditor::bufferRowForScreenRow(double screenRow) {
  return this->displayLayer->translateScreenPosition(Point(screenRow, 0)).row;
}

/*bufferRowsForScreenRows(startScreenRow, endScreenRow) {
  return this.displayLayer.bufferRowsForScreenRows(
    startScreenRow,
    endScreenRow + 1
  );
}*/

double TextEditor::screenRowForBufferRow(double row) {
  return this->displayLayer->translateBufferPosition(Point(row, 0)).row;
}

Point TextEditor::getRightmostScreenPosition() {
  return this->displayLayer->getRightmostScreenPosition();
}

/*getApproximateRightmostScreenPosition() {
  return this.displayLayer.getApproximateRightmostScreenPosition();
}*/

double TextEditor::getMaxScreenLineLength() {
  return this->getRightmostScreenPosition().column;
}

double TextEditor::getLongestScreenRow() {
  return this->getRightmostScreenPosition().row;
}

/*getApproximateLongestScreenRow() {
  return this.getApproximateRightmostScreenPosition().row;
}*/

double TextEditor::lineLengthForScreenRow(double screenRow) {
  return this->displayLayer->lineLengthForScreenRow(screenRow);
}

Range TextEditor::bufferRangeForBufferRow(double row, bool includeNewline) {
  return this->buffer->rangeForRow(row, includeNewline);
}

std::u16string TextEditor::getTextInRange(Range range) {
  return this->buffer->getTextInRange(range);
}

/*isBufferRowBlank(bufferRow) {
  return this.buffer.isRowBlank(bufferRow);
}*/

/*nextNonBlankBufferRow(bufferRow) {
  return this.buffer.nextNonBlankRow(bufferRow);
}*/

Point TextEditor::getEofBufferPosition() {
  return this->buffer->getEndPosition();
}

/*getCurrentParagraphBufferRange() {
  return this->getLastCursor().getCurrentParagraphBufferRange();
}*/

/*
Section: Mutating Text
*/

void TextEditor::setText(std::u16string &&text) {
  this->buffer->setText(std::move(text));
}

Range TextEditor::setTextInBufferRange(Range range, std::u16string &&text) {
  return this->getBuffer()->setTextInRange(range, std::move(text));
}
