#include "display-marker.h"
#include "display-marker-layer.h"
#include "marker.h"

/*
Section: Construction and Destruction
*/

DisplayMarker::DisplayMarker(DisplayMarkerLayer *layer, Marker *bufferMarker) {
  this->layer = layer;
  this->bufferMarker = bufferMarker;
  this->id = this->bufferMarker->id;
  this->hasChangeObservers = false;
}

DisplayMarker::~DisplayMarker() {}

void DisplayMarker::destroy() {
  //if (!this.isDestroyed()) {
    return this->bufferMarker->destroy();
  //}
}

void DisplayMarker::didDestroyBufferMarker() {
  this->didDestroyEmitter.emit();
  this->layer->didDestroyMarker(this);
  //this.emitter.dispose();
  //this.emitter.clear();
  //return (ref = this.bufferMarkerSubscription) != null ? ref.dispose() : void 0;
}

/*
Section: Event Subscription
*/

void DisplayMarker::onDidChange(std::function<void()> callback) {
  if (!this->hasChangeObservers) {
    this->bufferMarker->onDidChange([this]() {
      return this->notifyObservers();
    });
    this->hasChangeObservers = true;
  }
  return this->didChangeEmitter.on(callback);
}

void DisplayMarker::onDidDestroy(std::function<void()> callback) {
  //this.layer.markersWithDestroyListeners.add(this);
  return this->didDestroyEmitter.on(callback);
}

/*
Section: TextEditorMarker Details
*/

bool DisplayMarker::isReversed() const {
  return this->bufferMarker->isReversed();
}

/*
Section: Comparing to other markers
*/

int DisplayMarker::compare(const DisplayMarker *otherMarker) const {
  return this->bufferMarker->compare(otherMarker->bufferMarker);
}

bool DisplayMarker::isEqual(const DisplayMarker *other) const {
  return this->bufferMarker->isEqual(other->bufferMarker);
}

/*
Section: Managing the marker's range
*/

Range DisplayMarker::getBufferRange() const {
  return this->bufferMarker->getRange();
}

Range DisplayMarker::getScreenRange() {
  return this->layer->translateBufferRange(this->getBufferRange());
}

bool DisplayMarker::setBufferRange(const Range &bufferRange, optional<bool> reversed) {
  return this->bufferMarker->setRange(bufferRange, reversed);
}

bool DisplayMarker::setScreenRange(const Range &screenRange, optional<bool> reversed) {
  return this->setBufferRange(this->layer->translateScreenRange(screenRange), reversed);
}

Point DisplayMarker::getHeadBufferPosition() const {
  return this->bufferMarker->getHeadPosition();
}

bool DisplayMarker::setHeadBufferPosition(const Point &bufferPosition) {
  return this->bufferMarker->setHeadPosition(bufferPosition);
}

Point DisplayMarker::getHeadScreenPosition() {
  return this->layer->translateBufferPosition(this->bufferMarker->getHeadPosition());
}

bool DisplayMarker::setHeadScreenPosition(const Point &screenPosition, DisplayLayer::ClipDirection clipDirection) {
  return this->setHeadBufferPosition(this->layer->translateScreenPosition(screenPosition, clipDirection));
}

Point DisplayMarker::getTailBufferPosition() const {
  return this->bufferMarker->getTailPosition();
}

bool DisplayMarker::setTailBufferPosition(const Point &bufferPosition) {
  return this->bufferMarker->setTailPosition(bufferPosition);
}

Point DisplayMarker::getTailScreenPosition() {
  return this->layer->translateBufferPosition(this->bufferMarker->getTailPosition());
}

bool DisplayMarker::setTailScreenPosition(const Point &screenPosition) {
  return this->bufferMarker->setTailPosition(this->layer->translateScreenPosition(screenPosition));
}

Point DisplayMarker::getStartBufferPosition() const {
  return this->bufferMarker->getStartPosition();
}

Point DisplayMarker::getStartScreenPosition() {
  return this->layer->translateBufferPosition(this->getStartBufferPosition());
}

Point DisplayMarker::getEndBufferPosition() const {
  return this->bufferMarker->getEndPosition();
}

Point DisplayMarker::getEndScreenPosition() {
  return this->layer->translateBufferPosition(this->getEndBufferPosition());
}

bool DisplayMarker::hasTail() const {
  return this->bufferMarker->hasTail();
}

bool DisplayMarker::plantTail() {
  return this->bufferMarker->plantTail();
}

bool DisplayMarker::clearTail() {
  return this->bufferMarker->clearTail();
}

/*
Section: Private
*/

void DisplayMarker::notifyObservers() {
  return this->didChangeEmitter.emit();
}
