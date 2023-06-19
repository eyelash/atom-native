#ifndef LAYER_DECORATION_H_
#define LAYER_DECORATION_H_

#include "decoration.h"

struct DisplayMarkerLayer;
struct DecorationManager;

struct LayerDecoration {
  Decoration::Properties properties;

  LayerDecoration(DisplayMarkerLayer *, DecorationManager *, Decoration::Properties);
  ~LayerDecoration();

  Decoration::Properties getProperties();
};

#endif // LAYER_DECORATION_H_
