#ifndef MARKER_H_
#define MARKER_H_

#include <range.h>

class MarkerLayer;

class Marker {
  unsigned id;
  MarkerLayer *layer;

public:
  Marker(unsigned, MarkerLayer *, Range);
  ~Marker();

};

#endif  // MARKER_H_
