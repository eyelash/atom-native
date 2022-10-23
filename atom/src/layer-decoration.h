#ifndef LAYER_DECORATION_H_
#define LAYER_DECORATION_H_

class DisplayMarkerLayer;
class DecorationManager;

class LayerDecoration {
public:
  LayerDecoration(DisplayMarkerLayer *, DecorationManager *);
  ~LayerDecoration();
};

#endif  // LAYER_DECORATION_H_
