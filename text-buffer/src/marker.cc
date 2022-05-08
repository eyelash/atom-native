#include "marker.h"
#include "marker-layer.h"

Marker::Marker(unsigned id, MarkerLayer *layer, Range range, bool exclusivitySet) :
  layer{layer}, tailed{false}, reversed{false}, invalidate{InvalidationStrategy::overlap}, id{id} {
  if (!exclusivitySet) {
    this->layer->setMarkerIsExclusive(this->id, this->isExclusive());
  }
}

Marker::~Marker() {}

void Marker::destroy(bool suppressMarkerLayerUpdateEvents) {
  /*if (this.isDestroyed()) {
    return;
  }*/
  /*if (this.trackDestruction) {
    error = new Error();
    Error.captureStackTrace(error);
    this.destroyStackTrace = error.stack;
  }*/
  this->layer->destroyMarker(this, suppressMarkerLayerUpdateEvents);
  //this.emitter.emit('did-destroy');
  //return this.emitter.clear();
}

Range Marker::getRange() const {
  return this->layer->getMarkerRange(this->id);
}

bool Marker::setRange(const Range &range, optional<bool> reversed) {
  return this->update(this->getRange(), {
    range,
    reversed,
    true
  });
}

Point Marker::getHeadPosition() const {
  if (this->reversed) {
    return this->getStartPosition();
  } else {
    return this->getEndPosition();
  }
}

bool Marker::setHeadPosition(const Point &position) {
  Range oldRange = this->getRange();
  UpdateParams params;
  if (this->hasTail()) {
    if (this->isReversed()) {
      if (position.isLessThan(oldRange.end)) {
        params.range = Range(position, oldRange.end);
      } else {
        params.reversed = false;
        params.range = Range(oldRange.end, position);
      }
    } else {
      if (position.isLessThan(oldRange.start)) {
        params.reversed = true;
        params.range = Range(position, oldRange.start);
      } else {
        params.range = Range(oldRange.start, position);
      }
    }
  } else {
    params.range = Range(position, position);
  }
  return this->update(oldRange, params);
}

Point Marker::getTailPosition() const {
  if (this->reversed) {
    return this->getEndPosition();
  } else {
    return this->getStartPosition();
  }
}

bool Marker::setTailPosition(const Point &position) {
  Range oldRange = this->getRange();
  UpdateParams params;
  params.tailed = true;
  if (this->reversed) {
    if (position.isLessThan(oldRange.start)) {
      params.reversed = false;
      params.range = Range(position, oldRange.start);
    } else {
      params.range = Range(oldRange.start, position);
    }
  } else {
    if (position.isLessThan(oldRange.end)) {
      params.range = Range(position, oldRange.end);
    } else {
      params.reversed = true;
      params.range = Range(oldRange.end, position);
    }
  }
  return this->update(oldRange, params);
}

Point Marker::getStartPosition() const {
  return this->layer->getMarkerStartPosition(this->id);
}

Point Marker::getEndPosition() const {
  return this->layer->getMarkerEndPosition(this->id);
}

bool Marker::clearTail() {
  Point headPosition = this->getHeadPosition();
  return this->update(this->getRange(), {
    Range(headPosition, headPosition),
    false,
    false
  });
}

bool Marker::plantTail() {
  if (!this->hasTail()) {
    Point headPosition = this->getHeadPosition();
    UpdateParams params;
    params.range = Range(headPosition, headPosition);
    params.tailed = true;
    return this->update(this->getRange(), params);
  }
  return false;
}

bool Marker::isReversed() const {
  return this->tailed && this->reversed;
}

bool Marker::hasTail() const {
  return this->tailed;
}

bool Marker::isExclusive() {
  if (this->exclusive) {
    return *this->exclusive;
  } else {
    return this->getInvalidationStrategy() == InvalidationStrategy::inside || !this->hasTail();
  }
}

bool Marker::isEqual(const Marker *other) const {
  return this->invalidate == other->invalidate && this->tailed == other->tailed && this->reversed == other->reversed && this->exclusive == other->exclusive && this->getRange().isEqual(other->getRange());
}

Marker::InvalidationStrategy Marker::getInvalidationStrategy() const {
  return this->invalidate;
}

int Marker::compare(const Marker *other) const {
  return this->layer->compareMarkers(this->id, other->id);
}

bool Marker::update(const Range &oldRange, const UpdateParams &params) {
  bool wasExclusive = this->isExclusive();
  bool updated = false;
  if (params.range && !params.range->isEqual(oldRange)) {
    this->layer->setMarkerRange(this->id, *params.range);
    updated = true;
  }
  if (params.reversed && *params.reversed != this->reversed) {
    this->reversed = *params.reversed;
    updated = true;
  }
  if (params.tailed && *params.tailed != this->tailed) {
    this->tailed = *params.tailed;
    updated = true;
  }
  if (wasExclusive != this->isExclusive()) {
    this->layer->setMarkerIsExclusive(this->id, this->isExclusive());
    updated = true;
  }
  return updated;
}
