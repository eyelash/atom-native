#ifndef LAYER_DECORATION_H_
#define LAYER_DECORATION_H_

#include "decoration.h"

class DisplayMarkerLayer;
class DecorationManager;

class LayerDecoration {
  Decoration::Properties properties;

public:
  LayerDecoration(DisplayMarkerLayer *, DecorationManager *, Decoration::Properties);
  ~LayerDecoration();

  Decoration::Properties getProperties();
};

#endif  // LAYER_DECORATION_H_
