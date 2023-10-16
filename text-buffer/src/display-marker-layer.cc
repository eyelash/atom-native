#include "display-marker-layer.h"
#include "marker-layer.h"
#include "display-marker.h"
#include "marker.h"

DisplayMarkerLayer::DisplayMarkerLayer(DisplayLayer *displayLayer, MarkerLayer *bufferMarkerLayer, bool ownsBufferMarkerLayer) {
  this->displayLayer = displayLayer;
  this->bufferMarkerLayer = bufferMarkerLayer;
  this->id = this->bufferMarkerLayer->id;
  this->bufferMarkerLayer->displayMarkerLayers.insert(this);
  this->bufferMarkerLayer->onDidUpdate(std::bind(&DisplayMarkerLayer::emitDidUpdate, this));
}

DisplayMarkerLayer::~DisplayMarkerLayer() {
  for (auto &marker : this->markersById) {
    delete marker.second;
  }
}

/*
Section: Lifecycle
*/

void DisplayMarkerLayer::clear() {
  return this->bufferMarkerLayer->clear();
}

/*
Section: Event Subscription
*/

void DisplayMarkerLayer::onDidUpdate(std::function<void()> callback) {
  return this->didUpdateEmitter.on(callback);
}

void DisplayMarkerLayer::onDidCreateMarker(std::function<void(DisplayMarker *)> callback) {
  return this->bufferMarkerLayer->onDidCreateMarker([this, callback](Marker *bufferMarker) {
    callback(this->getMarker(bufferMarker->id));
  });
}

/*
Section: Marker creation
*/

DisplayMarker *DisplayMarkerLayer::markScreenRange(const Range &screenRange) {
  Range bufferRange = this->displayLayer->translateScreenRange(screenRange);
  Marker *marker = this->bufferMarkerLayer->markRange(bufferRange);
  return marker ? this->getMarker(marker->id) : nullptr;
}

DisplayMarker *DisplayMarkerLayer::markScreenPosition(const Point &screenPosition) {
  Point bufferPosition = this->displayLayer->translateScreenPosition(screenPosition);
  Marker *marker = this->bufferMarkerLayer->markPosition(bufferPosition);
  return marker ? this->getMarker(marker->id) : nullptr;
}

DisplayMarker *DisplayMarkerLayer::markBufferRange(const Range &bufferRange) {
  Marker *marker = this->bufferMarkerLayer->markRange(bufferRange);
  return marker ? this->getMarker(marker->id) : nullptr;
}

DisplayMarker *DisplayMarkerLayer::markBufferPosition(const Point &bufferPosition) {
  Marker *marker = this->bufferMarkerLayer->markPosition(bufferPosition);
  return marker ? this->getMarker(marker->id) : nullptr;
}

/*
Section: Querying
*/

DisplayMarker *DisplayMarkerLayer::getMarker(unsigned id) {
  if (DisplayMarker *displayMarker = get(this->markersById, id)) {
    return displayMarker;
  } else if (Marker *bufferMarker = this->bufferMarkerLayer->getMarker(id)) {
    return this->markersById[id] = new DisplayMarker(this, bufferMarker);
  }
  return nullptr;
}

/*getMarkers() {
  return this.bufferMarkerLayer.getMarkers().map(({id}) => {
    return this.getMarker(id);
  });
}*/

size_t DisplayMarkerLayer::getMarkerCount() {
  return this->bufferMarkerLayer->getMarkerCount();
}

DisplayMarkerLayer::FindParam startBufferPosition(const Point &position) {
  return [position](DisplayLayer *displayLayer) {
    return startPosition(position);
  };
}

DisplayMarkerLayer::FindParam endBufferPosition(const Point &position) {
  return [position](DisplayLayer *displayLayer) {
    return endPosition(position);
  };
}

DisplayMarkerLayer::FindParam startScreenPosition(const Point &position) {
  return [position](DisplayLayer *displayLayer) {
    return startPosition(displayLayer->translateScreenPosition(position));
  };
}

DisplayMarkerLayer::FindParam endScreenPosition(const Point &position) {
  return [position](DisplayLayer *displayLayer) {
    return endPosition(displayLayer->translateScreenPosition(position));
  };
}

DisplayMarkerLayer::FindParam startsInBufferRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return startsInRange(range);
  };
}

DisplayMarkerLayer::FindParam endsInBufferRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return endsInRange(range);
  };
}

DisplayMarkerLayer::FindParam startsInScreenRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return startsInRange(displayLayer->translateScreenRange(range));
  };
}

DisplayMarkerLayer::FindParam endsInScreenRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return endsInRange(displayLayer->translateScreenRange(range));
  };
}

DisplayMarkerLayer::FindParam startBufferRow(double row) {
  return [row](DisplayLayer *displayLayer) {
    return startRow(row);
  };
}

DisplayMarkerLayer::FindParam endBufferRow(double row) {
  return [row](DisplayLayer *displayLayer) {
    return endRow(row);
  };
}

DisplayMarkerLayer::FindParam startScreenRow(double row) {
  return [row](DisplayLayer *displayLayer) {
    const Point startBufferPosition = displayLayer->translateScreenPosition(Point(row, 0));
    const Point endBufferPosition = displayLayer->translateScreenPosition(Point(row, INFINITY));
    return startsInRange(Range(startBufferPosition, endBufferPosition));
  };
}

DisplayMarkerLayer::FindParam endScreenRow(double row) {
  return [row](DisplayLayer *displayLayer) {
    const Point startBufferPosition = displayLayer->translateScreenPosition(Point(row, 0));
    const Point endBufferPosition = displayLayer->translateScreenPosition(Point(row, INFINITY));
    return endsInRange(Range(startBufferPosition, endBufferPosition));
  };
}

DisplayMarkerLayer::FindParam intersectsBufferRowRange(std::pair<double, double> rowRange) {
  return [rowRange](DisplayLayer *displayLayer) {
    return intersectsRowRange(rowRange);
  };
}

DisplayMarkerLayer::FindParam intersectsScreenRowRange(std::pair<double, double> rowRange) {
  return [rowRange](DisplayLayer *displayLayer) {
    const double startScreenRow = rowRange.first;
    const double endScreenRow = rowRange.second;
    const Point startBufferPosition = displayLayer->translateScreenPosition(Point(startScreenRow, 0));
    const Point endBufferPosition = displayLayer->translateScreenPosition(Point(endScreenRow, INFINITY));
    return intersectsRange(Range(startBufferPosition, endBufferPosition));
  };
}

DisplayMarkerLayer::FindParam containsBufferRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return containsRange(range);
  };
}

DisplayMarkerLayer::FindParam containsScreenRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return containsRange(displayLayer->translateScreenRange(range));
  };
}

DisplayMarkerLayer::FindParam containsBufferPosition(const Point &position) {
  return [position](DisplayLayer *displayLayer) {
    return containsPosition(position);
  };
}

DisplayMarkerLayer::FindParam containsScreenPosition(const Point &position) {
  return [position](DisplayLayer *displayLayer) {
    return containsPosition(displayLayer->translateScreenPosition(position));
  };
}

DisplayMarkerLayer::FindParam containedInBufferRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return containedInRange(range);
  };
}

DisplayMarkerLayer::FindParam containedInScreenRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return containedInRange(displayLayer->translateScreenRange(range));
  };
}

DisplayMarkerLayer::FindParam intersectsBufferRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return intersectsRange(range);
  };
}

DisplayMarkerLayer::FindParam intersectsScreenRange(const Range &range) {
  return [range](DisplayLayer *displayLayer) {
    return intersectsRange(displayLayer->translateScreenRange(range));
  };
}

std::vector<DisplayMarker *> DisplayMarkerLayer::findMarkers(Slice<FindParam> params) {
  std::vector<MarkerLayer::FindParam> bufferMarkerLayerFindParams;
  for (const FindParam &param : params) {
    bufferMarkerLayerFindParams.push_back(param(this->displayLayer));
  }
  std::vector<DisplayMarker *> result;
  for (Marker *stringMarker : this->bufferMarkerLayer->findMarkers(bufferMarkerLayerFindParams)) {
    result.push_back(this->getMarker(stringMarker->id));
  }
  return result;
}

/*
Section: Private
*/
Point DisplayMarkerLayer::translateBufferPosition(const Point &bufferPosition, DisplayLayer::ClipDirection clipDirection) {
  return this->displayLayer->translateBufferPosition(bufferPosition, clipDirection);
}

Range DisplayMarkerLayer::translateBufferRange(const Range &bufferRange, DisplayLayer::ClipDirection clipDirection) {
  return this->displayLayer->translateBufferRange(bufferRange, clipDirection);
}

Point DisplayMarkerLayer::translateScreenPosition(const Point &screenPosition, DisplayLayer::ClipDirection clipDirection, bool skipSoftWrapIndentation) {
  return this->displayLayer->translateScreenPosition(screenPosition, clipDirection, skipSoftWrapIndentation);
}

Range DisplayMarkerLayer::translateScreenRange(const Range &screenRange, DisplayLayer::ClipDirection clipDirection, bool skipSoftWrapIndentation) {
  return this->displayLayer->translateScreenRange(screenRange, clipDirection, skipSoftWrapIndentation);
}

void DisplayMarkerLayer::emitDidUpdate() {
  return this->didUpdateEmitter.emit();
}

void DisplayMarkerLayer::destroyMarker(unsigned id) {
  auto iter = this->markersById.find(id);
  if (iter != this->markersById.end()) {
    return iter->second->didDestroyBufferMarker();
  }
}

void DisplayMarkerLayer::didDestroyMarker(DisplayMarker *marker) {
  //this.markersWithDestroyListeners.delete(marker);
  this->markersById.erase(marker->id);
  return delete marker;
}
