#ifndef DISPLAY_LAYER_H_
#define DISPLAY_LAYER_H_

#include <unordered_map>

class TextBuffer;
class DisplayMarkerLayer;

class DisplayLayer {
  TextBuffer *buffer;
  std::unordered_map<unsigned, DisplayMarkerLayer *> displayMarkerLayersById;

public:
  unsigned id;

  DisplayLayer(unsigned, TextBuffer *);
  ~DisplayLayer();

  DisplayMarkerLayer *addMarkerLayer();
};

#endif  // DISPLAY_LAYER_H_
