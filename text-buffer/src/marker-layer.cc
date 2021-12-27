#include "marker-layer.h"
#include "text-buffer.h"
#include "marker.h"

MarkerLayer::MarkerLayer(TextBuffer *delegate, unsigned id) :
  delegate{delegate},
  index{new MarkerIndex()},
  id{id} {}

MarkerLayer::~MarkerLayer() {
  for (auto& marker : this->markersById) {
    delete marker.second;
  }
  delete this->index;
}

void MarkerLayer::clear() {
  for (auto& marker : this->markersById) {
    delete marker.second;
  }
  this->markersById.clear();
  delete this->index;
  this->index = new MarkerIndex();
}

Marker *MarkerLayer::getMarker(unsigned id) {
  return this->markersById[id];
}

std::vector<Marker *> MarkerLayer::getMarkers() {
  std::vector<Marker *> results;
  for (auto &marker : this->markersById) {
    results.push_back(marker.second);
  }
  return results;
}

std::size_t MarkerLayer::getMarkerCount() {
  return this->markersById.size();
}

Marker *MarkerLayer::markRange(Range range) {
  // TODO: clip
  return this->createMarker(range);
}

Marker *MarkerLayer::markPosition(Point position) {
  // TODO: clip
  return this->createMarker(Range{position, position});
}

Marker *MarkerLayer::createMarker(Range range) {
  unsigned id = this->delegate->getNextMarkerId();
  Marker *marker = this->addMarker(id, range);
  return marker;
}

Marker *MarkerLayer::addMarker(unsigned id, Range range) {
  this->index->insert(id, range.start, range.end);
  return this->markersById[id] = new Marker(id, this, range);
}
