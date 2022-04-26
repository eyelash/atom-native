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
  return this->createMarker(this->delegate->clipRange(range));
}

Marker *MarkerLayer::markPosition(Point position) {
  position = this->delegate->clipPosition(position);
  return this->createMarker(Range{position, position});
}

void MarkerLayer::splice(Point start, Point oldExtent, Point newExtent) {
  auto invalidated = this->index->splice(start, oldExtent, newExtent);
  // TODO: destroy invalidated markers
}

Range MarkerLayer::getMarkerRange(unsigned id) {
  return this->index->get_range(id);
}

Point MarkerLayer::getMarkerStartPosition(unsigned id) {
  return this->index->get_start(id);
}

Point MarkerLayer::getMarkerEndPosition(unsigned id) {
  return this->index->get_end(id);
}

int MarkerLayer::compareMarkers(unsigned id1, unsigned id2) {
  return this->index->compare(id1, id2);
}

void MarkerLayer::setMarkerRange(unsigned id, Range range) {
  // TODO: clip
  this->index->remove(id);
  this->index->insert(id, range.start, range.end);
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
