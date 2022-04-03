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

  Range getRange();
  bool setRange(Range);

private:
  bool update(Range, Range);
};

#endif  // MARKER_H_
