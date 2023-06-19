#include "decoration.h"
#include <display-marker.h>

static unsigned nextId() {
  static unsigned idCounter = 0;
  return idCounter++;
}

static Decoration::Properties normalizeDecorationProperties(Decoration *decoration, Decoration::Properties decorationParams) {
  return decorationParams;
}

Decoration::Properties::Properties() {
  this->class_ = nullptr;
  this->onlyHead = false;
  this->onlyEmpty = false;
  this->onlyNonEmpty = false;
  this->omitEmptyLastRow = true;
}

Decoration::Properties::Properties(Decoration::Type type, const char *class_) {
  this->type = type;
  this->class_ = class_;
  this->onlyHead = false;
  this->onlyEmpty = false;
  this->onlyNonEmpty = false;
  this->omitEmptyLastRow = true;
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
  const Properties oldProperties = this->properties;
  this->properties = normalizeDecorationProperties(this, newProperties);
  /*if (newProperties.type != null) {
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
