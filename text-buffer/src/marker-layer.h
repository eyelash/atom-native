#ifndef MARKER_LAYER_H_
#define MARKER_LAYER_H_

#include "marker.h"
#include "range.h"
#include "event-kit.h"
#include "helpers.h"
#include <marker-index.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

struct TextBuffer;
struct DisplayMarkerLayer;

struct MarkerLayer {
  using Snapshot = std::unordered_map<unsigned, Marker::Snapshot>;

  TextBuffer *delegate;
  unsigned id;
  bool maintainHistory;
  Emitter<> didUpdateEmitter;
  Emitter<Marker *> didCreateMarkerEmitter;
  MarkerIndex *index;
  std::unordered_map<unsigned, Marker *> markersById;
  std::unordered_set<DisplayMarkerLayer *> displayMarkerLayers;

  MarkerLayer(TextBuffer *, unsigned, bool = false);
  ~MarkerLayer();

  void clear();
  Marker *getMarker(unsigned);
  std::vector<Marker *> getMarkers();
  size_t getMarkerCount();
  using FindParam = std::function<flat_set<unsigned>(MarkerIndex *)>;
  std::vector<Marker *> findMarkers(Slice<FindParam>);
  Marker *markRange(const Range &);
  Marker *markPosition(Point);
  void onDidUpdate(std::function<void()>);
  void onDidCreateMarker(std::function<void(Marker *)>);
  void splice(const Point &, const Point &, const Point &);
  void restoreFromSnapshot(const Snapshot &, bool = false);
  Snapshot createSnapshot();
  void emitChangeEvents(Snapshot &);
  void markerUpdated();
  void destroyMarker(Marker *, bool = false);
  bool hasMarker(unsigned);
  Range getMarkerRange(unsigned);
  Point getMarkerStartPosition(unsigned);
  Point getMarkerEndPosition(unsigned);
  int compareMarkers(unsigned, unsigned);
  void setMarkerRange(unsigned, const Range &);
  void setMarkerIsExclusive(unsigned, bool);
  Marker *createMarker(const Range &, const Marker::Params & = {}, bool = false);
  Marker *addMarker(unsigned, const Range &, const Marker::Params &);
  void emitUpdateEvent();
};

MarkerLayer::FindParam startPosition(const Point &);
MarkerLayer::FindParam endPosition(const Point &);
MarkerLayer::FindParam startsInRange(const Range &);
MarkerLayer::FindParam endsInRange(const Range &);
MarkerLayer::FindParam containsPoint(const Point &);
MarkerLayer::FindParam containsPosition(const Point &);
MarkerLayer::FindParam containsRange(const Range &);
MarkerLayer::FindParam intersectsRange(const Range &);
MarkerLayer::FindParam startRow(double);
MarkerLayer::FindParam endRow(double);
MarkerLayer::FindParam intersectsRow(double);
MarkerLayer::FindParam intersectsRowRange(std::pair<double, double>);
MarkerLayer::FindParam intersectsRowRange(double, double);
MarkerLayer::FindParam containedInRange(const Range &);

#endif // MARKER_LAYER_H_
