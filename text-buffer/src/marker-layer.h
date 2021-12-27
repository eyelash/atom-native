#ifndef MARKER_LAYER_H_
#define MARKER_LAYER_H_

#include <marker-index.h>
//#include <range.h>
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
  std::size_t getMarkerCount();
  Marker *markRange(Range);
  Marker *markPosition(Point);

private:
  Marker *createMarker(Range);
  Marker *addMarker(unsigned, Range);
};

#endif  // MARKER_LAYER_H_
