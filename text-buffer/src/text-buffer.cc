#include "text-buffer.h"
#include "marker-layer.h"
#include "display-layer.h"
#include "helpers.h"
#include "point-helpers.h"
#include "language-mode.h"

TextBuffer::TextBuffer() :
  buffer{new NativeTextBuffer()},
  nextMarkerLayerId{0},
  nextDisplayLayerId{0},
  defaultMarkerLayer{new MarkerLayer(this, this->nextMarkerLayerId++)},
  nextMarkerId{1},
  languageMode{new LanguageMode()},
  transactCallDepth{0} {
  this->markerLayers[this->defaultMarkerLayer->id] = this->defaultMarkerLayer;
}

TextBuffer::TextBuffer(const std::u16string &text) :
  buffer{new NativeTextBuffer(text)},
  nextMarkerLayerId{0},
  defaultMarkerLayer{new MarkerLayer(this, this->nextMarkerLayerId++)},
  nextMarkerId{1},
  languageMode{new LanguageMode()},
  transactCallDepth{0} {
  this->markerLayers[this->defaultMarkerLayer->id] = this->defaultMarkerLayer;
}

TextBuffer::~TextBuffer() {
  for (auto& displayLayer : this->displayLayers) {
    delete displayLayer.second;
  }
  for (auto& markerLayer : this->markerLayers) {
    delete markerLayer.second;
  }
  delete buffer;
}

/*
Section: File Details
*/

/*
Section: Reading Text
*/

bool TextBuffer::isEmpty() const {
  return this->buffer->size() == 0;
}

std::u16string TextBuffer::getText() {
  return buffer->text();
}

uint16_t TextBuffer::getCharacterAtPosition(Point position) {
  return this->buffer->character_at(position);
}

std::u16string TextBuffer::getTextInRange(Range range) {
  return this->buffer->text_in_range(range);
}

std::vector<std::u16string> TextBuffer::getLines() {
  std::vector<std::u16string> result;

  for (uint32_t row = 0, row_count = buffer->extent().row + 1; row < row_count; row++) {
    auto text = buffer->text_in_range({{row, 0}, {row, UINT32_MAX}});
    result.push_back(std::move(text));
  }

  return result;
}

optional<std::u16string> TextBuffer::getLastLine() {
  return this->lineForRow(this->getLastRow());
}

optional<std::u16string> TextBuffer::lineForRow(uint32_t row) {
  return this->buffer->line_for_row(row);
}

const char16_t *TextBuffer::lineEndingForRow(double row) {
  return this->buffer->line_ending_for_row(row);
}

optional<uint32_t> TextBuffer::lineLengthForRow(uint32_t row) {
  return this->buffer->line_length_for_row(row);
}

/*isRowBlank (row) {
  return !/\S/.test(this.lineForRow(row))
}*/

/*
Section: Mutating Text
*/

Range TextBuffer::setText(std::u16string text) {
  return this->setTextInRange(this->getRange(), std::move(text));
}

Range TextBuffer::setTextInRange(Range range, std::u16string newText) {
  const Range oldRange = this->clipRange(range);
  std::u16string oldText = this->getTextInRange(oldRange);

  Change change = {
    oldRange.start,
    oldRange.end,
    oldRange.start,
    traverse(oldRange.start, extentForText(newText)),
    std::move(oldText),
    std::move(newText)
  };
  const Range newRange = this->applyChange(std::move(change), true);
  return newRange;
}

Range TextBuffer::insert(Point position, std::u16string text) {
  return this->setTextInRange(Range(position, position), std::move(text));
}

Range TextBuffer::append(std::u16string text) {
  return this->insert(this->getEndPosition(), std::move(text));
}

Range TextBuffer::applyChange(Change change, bool pushToHistory) {
  const Point newStart = change.newStart;
  const Point newEnd = change.newEnd;
  const Point oldStart = change.oldStart;
  const Point oldEnd = change.oldEnd;
  std::u16string oldText = std::move(change.oldText);
  std::u16string newText = std::move(change.newText);

  const Point oldExtent = traversal(oldEnd, oldStart);
  const Range oldRange = Range(newStart, traverse(newStart, oldExtent));
  //oldRange.freeze()

  const Point newExtent = traversal(newEnd, newStart);
  const Range newRange = Range(newStart, traverse(newStart, newExtent));
  //newRange.freeze()

  /*if (pushToHistory) {
    if (!change.oldExtent) change.oldExtent = oldExtent
    if (!change.newExtent) change.newExtent = newExtent
    if (this.historyProvider) {
      this.historyProvider.pushChange(change)
    }
  }*/

  //const changeEvent = {oldRange, newRange, oldText, newText}
  for (auto &displayLayer : this->displayLayers) {
    displayLayer.second->bufferWillChange(oldRange);
  }

  //this.emitWillChangeEvent()
  this->buffer->set_text_in_range(oldRange, std::move(newText));

  for (auto &markerLayer : this->markerLayers) {
    markerLayer.second->splice(oldRange.start, oldExtent, newExtent);
    //this.markerLayersWithPendingUpdateEvents.add(markerLayer)
  }

  //this.cachedText = null
  //this.changesSinceLastDidChangeTextEvent.push(change)
  //this.changesSinceLastStoppedChangingEvent.push(change)
  this->emitDidChangeEvent(oldRange, newRange);
  return newRange;
}

void TextBuffer::emitDidChangeEvent(Range oldRange, Range newRange) {
  if (!oldRange.isEmpty() || !newRange.isEmpty()) {
    this->languageMode->bufferDidChange();
    for (auto &displayLayer : this->displayLayers) {
      displayLayer.second->bufferDidChange(oldRange, newRange);
    }
  }
}

Range TextBuffer::delete_(Range range) {
  return this->setTextInRange(range, u"");
}

Range TextBuffer::deleteRow(double row) {
  return this->deleteRows(row, row);
}

Range TextBuffer::deleteRows(double startRow, double endRow) {
  Point endPoint, startPoint;
  const double lastRow = this->getLastRow();

  if (startRow > endRow) { std::swap(startRow, endRow); }

  if (endRow < 0) {
    return Range(this->getFirstPosition(), this->getFirstPosition());
  }

  if (startRow > lastRow) {
    return Range(this->getEndPosition(), this->getEndPosition());
  }

  startRow = std::max(0.0, startRow);
  endRow = std::min(lastRow, endRow);

  if (endRow < lastRow) {
    startPoint = Point(startRow, 0);
    endPoint = Point(endRow + 1, 0);
  } else {
    if (startRow == 0) {
      startPoint = Point(startRow, 0);
    } else {
      startPoint = Point(startRow - 1, this->lineLengthForRow(startRow - 1));
    }
    endPoint = Point(endRow, this->lineLengthForRow(endRow));
  }

  return this->delete_(Range(startPoint, endPoint));
}

/*
Section: Markers
*/

MarkerLayer *TextBuffer::addMarkerLayer() {
  MarkerLayer *layer = new MarkerLayer(this, this->nextMarkerLayerId++);
  this->markerLayers[layer->id] = layer;
  return layer;
}

MarkerLayer *TextBuffer::getMarkerLayer(unsigned id) {
  return this->markerLayers[id];
}

MarkerLayer *TextBuffer::getDefaultMarkerLayer() {
  return this->defaultMarkerLayer;
}

Marker *TextBuffer::markRange(Range range) {
  return this->defaultMarkerLayer->markRange(range);
}

Marker *TextBuffer::markPosition(Point position) {
  return this->defaultMarkerLayer->markPosition(position);
}

std::vector<Marker *> TextBuffer::getMarkers() {
  return this->defaultMarkerLayer->getMarkers();
}

Marker *TextBuffer::getMarker(unsigned id) {
  return this->defaultMarkerLayer->getMarker(id);
}

std::size_t TextBuffer::getMarkerCount() {
  return this->defaultMarkerLayer->getMarkerCount();
}

/*
Section: History
*/

/*
Section: Search And Replace
*/

/*
Section: Buffer Range Details
*/

Range TextBuffer::getRange() const {
  return Range(this->getFirstPosition(), this->getEndPosition());
}

double TextBuffer::getLineCount() const { return this->buffer->extent().row + 1; }

unsigned TextBuffer::getLastRow() const {
  return this->getLineCount() - 1;
}

Point TextBuffer::getFirstPosition() const {
  return Point(0, 0);
}

Point TextBuffer::getEndPosition() const { return this->buffer->extent(); }

uint32_t TextBuffer::getLength() const { return this->buffer->size(); }

uint32_t TextBuffer::getMaxCharacterIndex() {
  return this->characterIndexForPosition({UINT32_MAX, UINT32_MAX});
}

Range TextBuffer::rangeForRow(unsigned row, bool includeNewline) {
  row = std::min(row, this->getLastRow());
  if (includeNewline && row < this->getLastRow()) {
    return Range(Point(row, 0), Point(row + 1, 0));
  } else {
    return Range(Point(row, 0), Point(row, this->lineLengthForRow(row)));
  }
}

uint32_t TextBuffer::characterIndexForPosition(Point position) {
  return this->buffer->clip_position(position).offset;
}

Point TextBuffer::positionForCharacterIndex(uint32_t offset) {
  return this->buffer->position_for_offset(offset);
}

Range TextBuffer::clipRange(Range range) {
  const Point start = this->clipPosition(range.start);
  const Point end = this->clipPosition(range.end);
  if (range.start == start && range.end == end) {
    return range;
  } else {
    return Range(start, end);
  }
}

Point TextBuffer::clipPosition(Point position) {
  const unsigned row = position.row, column = position.column;
  if (row < 0) {
    return this->getFirstPosition();
  } else if (row > this->getLastRow()) {
    return this->getEndPosition();
  } else if (column < 0) {
    return Point(row, 0);
  } else {
    const uint32_t lineLength = *this->lineLengthForRow(row);
    /*if (column >= lineLength && row < this.getLastRow() && options && options.clipDirection === 'forward') {
      return new Point(row + 1, 0)
    } else */ if (column > lineLength) {
      return Point(row, lineLength);
    } else {
      return position;
    }
  }
}

/*
Section: Buffer Operations
*/

/*
Section: Display Layers
*/

DisplayLayer *TextBuffer::addDisplayLayer() {
  const unsigned id = this->nextDisplayLayerId++;
  DisplayLayer *displayLayer = new DisplayLayer(id, this);
  this->displayLayers[id] = displayLayer;
  return displayLayer;
}

DisplayLayer *TextBuffer::getDisplayLayer(unsigned id) {
  return this->displayLayers[id];
}

/*
Language Modes
*/

/*
Section: Private Utility Methods
*/

/*
Section: Private History Delegate Methods
*/

/*
Section: Private MarkerLayer Delegate Methods
*/

void TextBuffer::markerCreated(MarkerLayer *layer, Marker *marker) {
  if (layer == this->defaultMarkerLayer) {
    //return this.emitter.emit('did-create-marker', marker)
  }
}

void TextBuffer::markersUpdated(MarkerLayer *layer) {
  if (this->transactCallDepth == 0) {
    //layer.emitUpdateEvent();
    if (layer == this->defaultMarkerLayer) {
      //return this.emitter.emit('did-update-markers')
    }
  } else {
    //return this.markerLayersWithPendingUpdateEvents.add(layer)
  }
}

unsigned TextBuffer::getNextMarkerId() { return this->nextMarkerId++; }
