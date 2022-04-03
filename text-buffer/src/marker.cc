#include "marker.h"
#include "marker-layer.h"

Marker::Marker(unsigned id, MarkerLayer *layer, Range range) :
  id{id}, layer{layer} {}

Marker::~Marker() {}

Range Marker::getRange() {
  return this->layer->getMarkerRange(this->id);
}

bool Marker::setRange(Range range) {
  return this->update(this->getRange(), range);
}

bool Marker::update(Range oldRange, Range range) {
  bool updated = false;
  if (!(range == oldRange)) {
    this->layer->setMarkerRange(this->id, range);
    updated = true;
  }
  return updated;
}
