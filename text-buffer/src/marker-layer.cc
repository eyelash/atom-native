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

/*
Section: Querying
*/

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

std::size_t MarkerLayer::getMarkerCount() const {
  return this->markersById.size();
}

MarkerLayer::FindParam startPosition(Point position) {
  return [position](MarkerIndex *index) {
    return index->find_starting_at(position);
  };
}

MarkerLayer::FindParam endPosition(Point position) {
  return [position](MarkerIndex *index) {
    return index->find_ending_at(position);
  };
}

MarkerLayer::FindParam startsInRange(Range range) {
  return [range](MarkerIndex *index) {
    return index->find_starting_in(range.start, range.end);
  };
}

MarkerLayer::FindParam endsInRange(Range range) {
  return [range](MarkerIndex *index) {
    return index->find_ending_in(range.start, range.end);
  };
}

MarkerLayer::FindParam containsPoint(Point position) {
  return containsPosition(position);
}

MarkerLayer::FindParam containsPosition(Point position) {
  return [position](MarkerIndex *index) {
    return index->find_containing(position, position);
  };
}

MarkerLayer::FindParam containsRange(Range range) {
  return [range](MarkerIndex *index) {
    return index->find_containing(range.start, range.end);
  };
}

MarkerLayer::FindParam intersectsRange(Range range) {
  return [range](MarkerIndex *index) {
    return index->find_intersecting(range.start, range.end);
  };
}

MarkerLayer::FindParam startRow(double row) {
  return [row](MarkerIndex *index) {
    return index->find_starting_in(Point(row, 0), Point(row, INFINITY));
  };
}

MarkerLayer::FindParam endRow(double row) {
  return [row](MarkerIndex *index) {
    return index->find_ending_in(Point(row, 0), Point(row, INFINITY));
  };
}

MarkerLayer::FindParam intersectsRow(double row) {
  return [row](MarkerIndex *index) {
    return index->find_intersecting(Point(row, 0), Point(row, INFINITY));
  };
}


MarkerLayer::FindParam intersectsRowRange(std::pair<double, double> rowRange) {
  return [rowRange](MarkerIndex *index) {
    return index->find_intersecting(Point(rowRange.first, 0), Point(rowRange.second, INFINITY));
  };
}

MarkerLayer::FindParam intersectsRowRange(double row0, double row1) {
  return [row0, row1](MarkerIndex *index) {
    return index->find_intersecting(Point(row0, 0), Point(row1, INFINITY));
  };
}

MarkerLayer::FindParam containedInRange(Range range) {
  return [range](MarkerIndex *index) {
    return index->find_contained_in(range.start, range.end);
  };
}

std::vector<Marker *> MarkerLayer::findMarkers(FindParam param) {
  auto markerIds = param(this->index);
  std::vector<Marker *> result;
  for (unsigned markerId : markerIds) {
    Marker *marker = this->markersById[markerId];
    result.push_back(marker);
  }
  std::sort(result.begin(), result.end(), [](Marker *a, Marker *b) {
    return a->compare(b) < 0;
  });
  return result;
}

Marker *MarkerLayer::markRange(Range range) {
  return this->createMarker(this->delegate->clipRange(range));
}

Marker *MarkerLayer::markPosition(Point position) {
  position = this->delegate->clipPosition(position);
  return this->createMarker(Range{position, position});
}

/*
Section: Event subscription
*/

void MarkerLayer::onDidCreateMarker(std::function<void(Marker *)> callback) {
  //this->emitCreateMarkerEvents = true;
  return this->didCreateMarkerEmitter.on(callback);
}

/*
Section: Private - TextBuffer interface
*/

void MarkerLayer::splice(Point start, Point oldExtent, Point newExtent) {
  auto invalidated = this->index->splice(start, oldExtent, newExtent);
  // TODO: destroy invalidated markers
}

Range MarkerLayer::getMarkerRange(unsigned id) const {
  return this->index->get_range(id);
}

Point MarkerLayer::getMarkerStartPosition(unsigned id) const {
  return this->index->get_start(id);
}

Point MarkerLayer::getMarkerEndPosition(unsigned id) const {
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

Marker *MarkerLayer::createMarker(const Range &range) {
  unsigned id = this->delegate->getNextMarkerId();
  Marker *marker = this->addMarker(id, range);
  this->didCreateMarkerEmitter.emit(marker);
  return marker;
}

Marker *MarkerLayer::addMarker(unsigned id, const Range &range) {
  this->index->insert(id, range.start, range.end);
  return this->markersById[id] = new Marker(id, this, range);
}
