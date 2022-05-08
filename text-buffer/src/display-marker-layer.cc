#include "display-marker-layer.h"
#include "marker-layer.h"
#include "display-marker.h"
#include "marker.h"

DisplayMarkerLayer::DisplayMarkerLayer(DisplayLayer *displayLayer, MarkerLayer *bufferMarkerLayer, bool ownsBufferMarkerLayer) :
  displayLayer{displayLayer},
  bufferMarkerLayer{bufferMarkerLayer},
  id{bufferMarkerLayer->id} {
  this->bufferMarkerLayer->displayMarkerLayers.insert(this);
}

DisplayMarkerLayer::~DisplayMarkerLayer() {}

/*
Section: Lifecycle
*/

void DisplayMarkerLayer::clear() {
  return this->bufferMarkerLayer->clear();
}

/*
Section: Event Subscription
*/

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
