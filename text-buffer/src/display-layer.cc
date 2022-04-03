#include "display-layer.h"
#include "text-buffer.h"
#include "display-marker-layer.h"

DisplayLayer::DisplayLayer(unsigned id, TextBuffer *buffer) :
  buffer{buffer}, id{id} {}

DisplayLayer::~DisplayLayer() {
  for (auto& displayMarkerLayer : this->displayMarkerLayersById) {
    delete displayMarkerLayer.second;
  }
}

DisplayMarkerLayer *DisplayLayer::addMarkerLayer() {
  DisplayMarkerLayer *markerLayer = new DisplayMarkerLayer(this, this->buffer->addMarkerLayer(), true);
  this->displayMarkerLayersById[markerLayer->id] = markerLayer;
  return markerLayer;
}
