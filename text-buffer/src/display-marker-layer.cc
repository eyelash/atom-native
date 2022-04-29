#include "display-marker-layer.h"
#include "display-layer.h"
#include "marker-layer.h"
#include "display-marker.h"
#include "marker.h"

DisplayMarkerLayer::DisplayMarkerLayer(DisplayLayer *displayLayer, MarkerLayer *bufferMarkerLayer, bool ownsBufferMarkerLayer) :
  displayLayer{displayLayer},
  bufferMarkerLayer{bufferMarkerLayer},
  id{bufferMarkerLayer->id} {}

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
Point DisplayMarkerLayer::translateBufferPosition(Point bufferPosition) {
  return this->displayLayer->translateBufferPosition(bufferPosition);
}

Range DisplayMarkerLayer::translateBufferRange(Range bufferRange) {
  return this->displayLayer->translateBufferRange(bufferRange);
}

Point DisplayMarkerLayer::translateScreenPosition(Point screenPosition) {
  return this->displayLayer->translateScreenPosition(screenPosition);
}

Range DisplayMarkerLayer::translateScreenRange(Range screenRange) {
  return this->displayLayer->translateScreenRange(screenRange);
}
