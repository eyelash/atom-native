#include "layer-decoration.h"

LayerDecoration::LayerDecoration(DisplayMarkerLayer *, DecorationManager *, Decoration::Properties properties) {
  this->properties = properties;
}

LayerDecoration::~LayerDecoration() {}

Decoration::Properties LayerDecoration::getProperties() {
  return this->properties;
}
