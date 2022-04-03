#ifndef DISPLAY_MARKER_H_
#define DISPLAY_MARKER_H_

#include <range.h>

class DisplayMarkerLayer;
class Marker;

class DisplayMarker {
  DisplayMarkerLayer *layer;
  Marker *bufferMarker;

public:
  DisplayMarker(DisplayMarkerLayer *, Marker *bufferMarker);
  ~DisplayMarker();

  Range getBufferRange();
  bool setBufferRange(Range);
};

#endif  // DISPLAY_MARKER_H_
