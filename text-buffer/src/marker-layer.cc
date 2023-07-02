#include "marker-layer.h"
#include "text-buffer.h"
#include "display-marker-layer.h"
#include "set-helpers.h"

/*
Section: Lifecycle
*/

MarkerLayer::MarkerLayer(TextBuffer *delegate, unsigned id) {
  this->delegate = delegate;
  this->id = id;
  this->index = new MarkerIndex();
}

MarkerLayer::~MarkerLayer() {
  for (auto &marker : this->markersById) {
    delete marker.second;
  }
  delete this->index;
}

void MarkerLayer::clear() {
  for (auto &marker : this->markersById) {
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
  return this->markersById.count(id) ? this->markersById[id] : nullptr;
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

std::vector<Marker *> MarkerLayer::findMarkers(Slice<FindParam> params) {
  std::vector<Marker *> result;
  auto iter = params.begin();
  if (iter != params.end()) {
    auto markerIds = (*iter)(this->index);
    ++iter;
    while (iter != params.end()) {
      intersectSet(markerIds, (*iter)(this->index));
      ++iter;
    }
    for (unsigned markerId : markerIds) {
      Marker *marker = this->markersById[markerId];
      result.push_back(marker);
    }
  } else {
    for (auto& marker : this->markersById) {
      result.push_back(marker.second);
    }
  }
  std::sort(result.begin(), result.end(), [](Marker *a, Marker *b) {
    return a->compare(b) < 0;
  });
  return result;
}

/*
Section: Marker creation
*/

unsigned MarkerLayer::markRange(Range range) {
  return this->createMarker(this->delegate->clipRange(range));
}

unsigned MarkerLayer::markPosition(Point position) {
  position = this->delegate->clipPosition(position);
  return this->createMarker(Range{position, position});
}

/*
Section: Event subscription
*/

void MarkerLayer::onDidUpdate(std::function<void()> callback) {
  return this->didUpdateEmitter.on(callback);
}

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

template <typename K, typename V> static std::vector<K> keys(const std::unordered_map<K, V> &map) {
  std::vector<K> result;
  for (const auto &entry : map) {
    result.push_back(entry.first);
  }
  return result;
}

void MarkerLayer::restoreFromSnapshot(const Snapshot &snapshots, bool alwaysCreate) {
  auto existingMarkerIds = keys(this->markersById);
  for (const auto &snapshot : snapshots) {
    const unsigned id = snapshot.first;
    /*if (alwaysCreate) {
      this.createMarker(snapshot.range, snapshot, true);
      continue;
    }*/
    auto iter = this->markersById.find(id);
    if (iter != this->markersById.end()) {
      Marker *marker = iter != this->markersById.end() ? iter->second : nullptr;
      marker->update(marker->getRange(), {snapshot.second.range, snapshot.second.reversed, snapshot.second.tailed}, true, true);
    } else {
      //Marker *marker = snapshot.marker;
      /* if (marker) {
        this.markersById[marker.id] = marker;
        ({range} = snapshot);
        this.index.insert(marker.id, range.start, range.end);
        marker.update(marker.getRange(), snapshot, true, true);
        if (this.emitCreateMarkerEvents) {
          this.emitter.emit('did-create-marker', marker);
        }
      } else */ {
        this->createMarker(snapshot.second.range, {snapshot.second.range, snapshot.second.reversed, snapshot.second.tailed}, true);
      }
    }
  }
  for (unsigned id : existingMarkerIds) {
    if (this->markersById.count(id) && snapshots.count(id) == 0) {
      Marker *marker = this->markersById[id];
      marker->destroy(true);
    }
  }
}

MarkerLayer::Snapshot MarkerLayer::createSnapshot() {
  Snapshot result;
  auto ranges = this->index->dump();
  for (auto &marker : this->markersById) {
    const unsigned id = marker.first;
    result[id] = marker.second->getSnapshot(ranges[id]);
  }
  return result;
}

void MarkerLayer::emitChangeEvents(Snapshot &snapshot) {
  // TODO: markersWithChangeListeners
  for (auto &marker : this->markersById) {
    marker.second->emitChangeEvent(snapshot[marker.second->id].range, true, false);
  }
}

/*
Section: Private - Marker interface
*/

void MarkerLayer::markerUpdated() {
  return this->delegate->markersUpdated(this);
}

void MarkerLayer::destroyMarker(Marker *marker, bool suppressMarkerLayerUpdateEvents) {
  if (this->markersById.count(marker->id)) {
    this->markersById.erase(marker->id);
    this->index->remove(marker->id);
    //this.markersWithChangeListeners.delete(marker);
    //this.markersWithDestroyListeners.delete(marker);
    for (DisplayMarkerLayer *displayMarkerLayer : this->displayMarkerLayers) {
      displayMarkerLayer->destroyMarker(marker->id);
    }
    delete marker;
    if (!suppressMarkerLayerUpdateEvents) {
      return this->delegate->markersUpdated(this);
    }
  }
}

bool MarkerLayer::hasMarker(unsigned id) {
  return /* !this->destroyed && */ this->index->has(id);
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

void MarkerLayer::setMarkerRange(unsigned id, const Range &range) {
  Point start = range.start, end = range.end;
  start = this->delegate->clipPosition(start);
  end = this->delegate->clipPosition(end);
  this->index->remove(id);
  this->index->insert(id, start, end);
}

void MarkerLayer::setMarkerIsExclusive(unsigned id, bool exclusive) {
  this->index->set_exclusive(id, exclusive);
}

unsigned MarkerLayer::createMarker(const Range &range, const Marker::Params &params, bool suppressMarkerLayerUpdateEvents) {
  unsigned id = this->delegate->getNextMarkerId();
  Marker *marker = this->addMarker(id, range, params);
  this->delegate->markerCreated(this, marker);
  if (!suppressMarkerLayerUpdateEvents) {
    this->delegate->markersUpdated(this);
  }
  //marker.trackDestruction = (ref = this.trackDestructionInOnDidCreateMarkerCallbacks) != null ? ref : false;
  //if (this->emitCreateMarkerEvents) {
    this->didCreateMarkerEmitter.emit(marker);
  //}
  //marker.trackDestruction = false;
  return id;
}

/*
Section: Internal
*/

Marker *MarkerLayer::addMarker(unsigned id, const Range &range, const Marker::Params &params) {
  this->index->insert(id, range.start, range.end);
  return this->markersById[id] = new Marker(id, this, range, params);
}

void MarkerLayer::emitUpdateEvent() {
  return this->didUpdateEmitter.emit();
}
