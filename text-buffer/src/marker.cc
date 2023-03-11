#include "marker.h"
#include "marker-layer.h"

Marker::Marker(unsigned id, MarkerLayer *layer, Range range, const Params &params, bool exclusivitySet) {
  this->id = id;
  this->layer = layer;
  this->tailed = params.tailed ? *params.tailed : true;
  this->reversed = params.reversed ? *params.reversed : false;
  this->invalidate = InvalidationStrategy::overlap;
  if (!exclusivitySet) {
    this->layer->setMarkerIsExclusive(this->id, this->isExclusive());
  }
}

Marker::~Marker() {}

/*
Section: Event Subscription
*/

void Marker::onDidDestroy(std::function<void()> callback) {
  //this.layer.markersWithDestroyListeners.add(this);
  this->didDestroyEmitter.on(callback);
}

void Marker::onDidChange(std::function<void()> callback) {
  return this->didChangeEmitter.on(callback);
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
  Params params;
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
  Params params;
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
    Params params;
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
  //this->didDestroyEmitter.emit();
  //return this.emitter.clear();
}

int Marker::compare(const Marker *other) const {
  return this->layer->compareMarkers(this->id, other->id);
}

bool Marker::update(const Range &oldRange, const Params &params, bool textChanged, bool suppressMarkerLayerUpdateEvents) {
  bool wasExclusive = this->isExclusive();
  bool updated = false, propertiesChanged = false;
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
  this->emitChangeEvent(params.range ? *params.range : oldRange, textChanged, propertiesChanged);
  if (updated && !suppressMarkerLayerUpdateEvents) {
    this->layer->markerUpdated();
  }
  return updated;
}

Marker::Snapshot Marker::getSnapshot(Range range, bool includeMarker) {
  Snapshot snapshot {range, this->reversed, this->tailed, this->invalidate, this->exclusive};
  if (includeMarker) {
    //snapshot.marker = this;
  }
  return snapshot;
}

/*
Section: Private
*/

void Marker::emitChangeEvent(Range currentRange, bool textChanged, bool propertiesChanged) {
  this->didChangeEmitter.emit();
}
