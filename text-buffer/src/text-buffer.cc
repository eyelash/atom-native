#include "text-buffer.h"
#include "marker-layer.h"

TextBuffer::TextBuffer() :
  buffer{new NativeTextBuffer()},
  nextMarkerLayerId{0},
  defaultMarkerLayer{new MarkerLayer(this, this->nextMarkerLayerId++)},
  nextMarkerId{1} {
  this->markerLayers[this->defaultMarkerLayer->id] = this->defaultMarkerLayer;
}

TextBuffer::TextBuffer(const std::u16string &text) :
  buffer{new NativeTextBuffer(text)},
  nextMarkerLayerId{0},
  defaultMarkerLayer{new MarkerLayer(this, this->nextMarkerLayerId++)},
  nextMarkerId{1} {
  this->markerLayers[this->defaultMarkerLayer->id] = this->defaultMarkerLayer;
}

TextBuffer::~TextBuffer() {
  for (auto& markerLayer : this->markerLayers) {
    delete markerLayer.second;
  }
  delete buffer;
}

std::u16string TextBuffer::getText() {
  return buffer->text();
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
Section: Private MarkerLayer Delegate Methods
*/

unsigned TextBuffer::getNextMarkerId() { return this->nextMarkerId++; }
