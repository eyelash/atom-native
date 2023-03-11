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

DisplayMarker *DisplayMarkerLayer::markScreenRange(Range screenRange) {
  Range bufferRange = this->displayLayer->translateScreenRange(screenRange);
  return this->getMarker(this->bufferMarkerLayer->markRange(bufferRange)->id);
}

DisplayMarker *DisplayMarkerLayer::markScreenPosition(Point screenPosition) {
  Point bufferPosition = this->displayLayer->translateScreenPosition(screenPosition);
  return this->getMarker(this->bufferMarkerLayer->markPosition(bufferPosition)->id);
}

DisplayMarker *DisplayMarkerLayer::markBufferRange(Range bufferRange) {
  return this->getMarker(this->bufferMarkerLayer->markRange(bufferRange)->id);
}

DisplayMarker *DisplayMarkerLayer::markBufferPosition(Point bufferPosition) {
  return this->getMarker(this->bufferMarkerLayer->markPosition(bufferPosition)->id);
}

/*
Section: Querying
*/

DisplayMarker *DisplayMarkerLayer::getMarker(unsigned id) {
  if (DisplayMarker *displayMarker = this->markersById[id]) {
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

std::size_t DisplayMarkerLayer::getMarkerCount() const {
  return this->bufferMarkerLayer->getMarkerCount();
}

DisplayMarkerLayer::FindParam startBufferPosition(Point position) {
  return [position](DisplayLayer *displayLayer) {
    return startPosition(position);
  };
}

DisplayMarkerLayer::FindParam endBufferPosition(Point position) {
  return [position](DisplayLayer *displayLayer) {
    return endPosition(position);
  };
}

DisplayMarkerLayer::FindParam startScreenPosition(Point position) {
  return [position](DisplayLayer *displayLayer) {
    return startPosition(displayLayer->translateScreenPosition(position));
  };
}

DisplayMarkerLayer::FindParam endScreenPosition(Point position) {
  return [position](DisplayLayer *displayLayer) {
    return endPosition(displayLayer->translateScreenPosition(position));
  };
}

DisplayMarkerLayer::FindParam startsInBufferRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return startsInRange(range);
  };
}

DisplayMarkerLayer::FindParam endsInBufferRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return endsInRange(range);
  };
}

DisplayMarkerLayer::FindParam startsInScreenRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return startsInRange(displayLayer->translateScreenRange(range));
  };
}

DisplayMarkerLayer::FindParam endsInScreenRange(Range range) {
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

DisplayMarkerLayer::FindParam containsBufferRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return containsRange(range);
  };
}

DisplayMarkerLayer::FindParam containsScreenRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return containsRange(displayLayer->translateScreenRange(range));
  };
}

DisplayMarkerLayer::FindParam containsBufferPosition(Point position) {
  return [position](DisplayLayer *displayLayer) {
    return containsPosition(position);
  };
}

DisplayMarkerLayer::FindParam containsScreenPosition(Point position) {
  return [position](DisplayLayer *displayLayer) {
    return containsPosition(displayLayer->translateScreenPosition(position));
  };
}

DisplayMarkerLayer::FindParam containedInBufferRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return containedInRange(range);
  };
}

DisplayMarkerLayer::FindParam containedInScreenRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return containedInRange(displayLayer->translateScreenRange(range));
  };
}

DisplayMarkerLayer::FindParam intersectsBufferRange(Range range) {
  return [range](DisplayLayer *displayLayer) {
    return intersectsRange(range);
  };
}

DisplayMarkerLayer::FindParam intersectsScreenRange(Range range) {
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
Point DisplayMarkerLayer::translateBufferPosition(Point bufferPosition, DisplayLayer::ClipDirection clipDirection) {
  return this->displayLayer->translateBufferPosition(bufferPosition, clipDirection);
}

Range DisplayMarkerLayer::translateBufferRange(Range bufferRange, DisplayLayer::ClipDirection clipDirection) {
  return this->displayLayer->translateBufferRange(bufferRange, clipDirection);
}

Point DisplayMarkerLayer::translateScreenPosition(Point screenPosition, DisplayLayer::ClipDirection clipDirection, bool skipSoftWrapIndentation) {
  return this->displayLayer->translateScreenPosition(screenPosition, clipDirection, skipSoftWrapIndentation);
}

Range DisplayMarkerLayer::translateScreenRange(Range screenRange, DisplayLayer::ClipDirection clipDirection, bool skipSoftWrapIndentation) {
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
