#include "display-marker.h"
#include "marker.h"

DisplayMarker::DisplayMarker(DisplayMarkerLayer *layer, Marker *bufferMarker) :
  layer{layer},
  bufferMarker{bufferMarker} {}

DisplayMarker::~DisplayMarker() {}

Range DisplayMarker::getBufferRange() {
  return this->bufferMarker->getRange();
}

bool DisplayMarker::setBufferRange(Range bufferRange) {
  return this->bufferMarker->setRange(bufferRange);
}
