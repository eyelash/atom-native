#ifndef DISPLAY_MARKER_LAYER_H_
#define DISPLAY_MARKER_LAYER_H_

#include "range.h"
#include <unordered_map>
#include <functional>

class DisplayLayer;
class MarkerLayer;
class DisplayMarker;

class DisplayMarkerLayer {
  DisplayLayer *displayLayer;
  MarkerLayer *bufferMarkerLayer;
  std::unordered_map<unsigned, DisplayMarker *> markersById;

public:
  unsigned id;

  DisplayMarkerLayer(DisplayLayer *, MarkerLayer *, bool);
  ~DisplayMarkerLayer();

  void clear();
  void onDidCreateMarker(std::function<void(DisplayMarker *)>);
  DisplayMarker *markScreenRange(Range);
  DisplayMarker *markScreenPosition(Point);
  DisplayMarker *markBufferRange(Range);
  DisplayMarker *markBufferPosition(Point);
  DisplayMarker *getMarker(unsigned);
  std::size_t getMarkerCount() const;
  Point translateBufferPosition(Point);
  Range translateBufferRange(Range);
  Point translateScreenPosition(Point);
  Range translateScreenRange(Range);
};

#endif  // DISPLAY_MARKER_LAYER_H_
