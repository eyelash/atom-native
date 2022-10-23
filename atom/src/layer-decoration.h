#ifndef LAYER_DECORATION_H_
#define LAYER_DECORATION_H_

#include "decoration.h"

class DisplayMarkerLayer;
class DecorationManager;

class LayerDecoration {
public:
  LayerDecoration(DisplayMarkerLayer *, DecorationManager *, Decoration::Properties);
  ~LayerDecoration();
};

#endif  // LAYER_DECORATION_H_
