#ifndef DISPLAY_MARKER_LAYER_H_
#define DISPLAY_MARKER_LAYER_H_

class DisplayLayer;
class MarkerLayer;

class DisplayMarkerLayer {
  DisplayLayer *displayLayer;

public:
  unsigned id;

  DisplayMarkerLayer(DisplayLayer *, MarkerLayer *, bool);
  ~DisplayMarkerLayer();

};

#endif  // DISPLAY_MARKER_LAYER_H_
