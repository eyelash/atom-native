#ifndef MARKER_LAYER_H_
#define MARKER_LAYER_H_

#include "range.h"
#include "event-kit.h"
#include "helpers.h"
#include <marker-index.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

class TextBuffer;
class Marker;
class DisplayMarkerLayer;

class MarkerLayer {
  TextBuffer *delegate;
  Emitter<Marker *> didCreateMarkerEmitter;
  MarkerIndex *index;
  std::unordered_map<unsigned, Marker *> markersById;

public:
  unsigned id;
  std::unordered_set<DisplayMarkerLayer *> displayMarkerLayers;

  MarkerLayer(TextBuffer *, unsigned);
  ~MarkerLayer();

  void clear();
  Marker *getMarker(unsigned);
  std::vector<Marker *> getMarkers();
  std::size_t getMarkerCount() const;
  using FindParam = std::function<flat_set<unsigned>(MarkerIndex *)>;
  std::vector<Marker *> findMarkers(Slice<FindParam>);
  Marker *markRange(Range);
  Marker *markPosition(Point);
  void onDidCreateMarker(std::function<void(Marker *)>);
  void splice(Point, Point, Point);
  void destroyMarker(Marker *, bool = false);
  bool hasMarker(unsigned);
  Range getMarkerRange(unsigned) const;
  Point getMarkerStartPosition(unsigned) const;
  Point getMarkerEndPosition(unsigned) const;
  int compareMarkers(unsigned, unsigned);
  void setMarkerRange(unsigned, const Range &);
  void setMarkerIsExclusive(unsigned, bool);

private:
  Marker *createMarker(const Range &, bool = false);
  Marker *addMarker(unsigned, const Range &);
};

MarkerLayer::FindParam startPosition(Point);
MarkerLayer::FindParam endPosition(Point);
MarkerLayer::FindParam startsInRange(Range);
MarkerLayer::FindParam endsInRange(Range);
MarkerLayer::FindParam containsPoint(Point);
MarkerLayer::FindParam containsPosition(Point);
MarkerLayer::FindParam containsRange(Range);
MarkerLayer::FindParam intersectsRange(Range);
MarkerLayer::FindParam startRow(double);
MarkerLayer::FindParam endRow(double);
MarkerLayer::FindParam intersectsRow(double);
MarkerLayer::FindParam intersectsRowRange(std::pair<double, double>);
MarkerLayer::FindParam intersectsRowRange(double, double);
MarkerLayer::FindParam containedInRange(Range);

#endif  // MARKER_LAYER_H_
