#include "decoration.h"
#include <display-marker.h>

static unsigned nextId() {
  static unsigned idCounter = 0;
  return idCounter++;
}

static void normalizeDecorationProperties() {

}

Decoration::Decoration(DisplayMarker *marker, DecorationManager *decorationManager, Properties properties) {
  this->marker = marker;
  this->decorationManager = decorationManager;
  this->setProperties(properties);
  this->marker->onDidDestroy([this]() {
    this->destroy();
  });
}

Decoration::~Decoration() {}

void Decoration::destroy() {
  //this.decorationManager.didDestroyMarkerDecoration(this);
  //this.emitter.emit('did-destroy');
}

/*
Section: Event Subscription
*/

/*
Section: Decoration Details
*/

/*
Section: Properties
*/

Decoration::Properties Decoration::getProperties() {
  return this->properties;
}

void Decoration::setProperties(Properties newProperties) {
  /*if (this.destroyed) {
    return;
  }*/
  /*const Properties oldProperties = this->properties;
  this.properties = normalizeDecorationProperties(this, newProperties);
  if (newProperties.type != null) {
    this.decorationManager.decorationDidChangeType(this);
  }
  this.decorationManager.emitDidUpdateDecorations();
  return this.emitter.emit('did-change-properties', {
    oldProperties,
    newProperties
  });*/
}

/*
Section: Utility
*/

/*
Section: Private methods
*/
