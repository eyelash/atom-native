#ifndef MARKER_LAYER_H_
#define MARKER_LAYER_H_

#include "range.h"
#include <marker-index.h>
#include <vector>
#include <unordered_map>

class TextBuffer;
class Marker;

class MarkerLayer {
  TextBuffer *delegate;
  MarkerIndex *index;
  std::unordered_map<unsigned, Marker *> markersById;

public:
  unsigned id;

  MarkerLayer(TextBuffer *, unsigned);
  ~MarkerLayer();

  void clear();
  Marker *getMarker(unsigned);
  std::vector<Marker *> getMarkers();
  std::size_t getMarkerCount() const;
  Marker *markRange(Range);
  Marker *markPosition(Point);
  void splice(Point, Point, Point);
  Range getMarkerRange(unsigned) const;
  Point getMarkerStartPosition(unsigned) const;
  Point getMarkerEndPosition(unsigned) const;
  int compareMarkers(unsigned, unsigned);
  void setMarkerRange(unsigned, Range);

private:
  Marker *createMarker(const Range &);
  Marker *addMarker(unsigned, const Range &);
};

#endif  // MARKER_LAYER_H_
