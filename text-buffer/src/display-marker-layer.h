#ifndef DISPLAY_MARKER_LAYER_H_
#define DISPLAY_MARKER_LAYER_H_

#include "range.h"
#include "display-layer.h"
#include "marker-layer.h"
#include "event-kit.h"
#include <unordered_map>
#include <functional>

struct DisplayMarker;

struct DisplayMarkerLayer {
  DisplayLayer *displayLayer;
  MarkerLayer *bufferMarkerLayer;
  unsigned id;
  std::unordered_map<unsigned, DisplayMarker *> markersById;
  Emitter<> didUpdateEmitter;

  DisplayMarkerLayer(DisplayLayer *, MarkerLayer *, bool);
  ~DisplayMarkerLayer();

  void clear();
  void onDidUpdate(std::function<void()>);
  void onDidCreateMarker(std::function<void(DisplayMarker *)>);
  DisplayMarker *markScreenRange(const Range &);
  DisplayMarker *markScreenPosition(const Point &);
  DisplayMarker *markBufferRange(const Range &);
  DisplayMarker *markBufferPosition(const Point &);
  DisplayMarker *getMarker(unsigned);
  size_t getMarkerCount();
  using FindParam = std::function<MarkerLayer::FindParam(DisplayLayer *)>;
  std::vector<DisplayMarker *> findMarkers(Slice<FindParam>);
  Point translateBufferPosition(const Point &, DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest);
  Range translateBufferRange(const Range &, DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest);
  Point translateScreenPosition(const Point &, DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest, bool = false);
  Range translateScreenRange(const Range &, DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest, bool = false);
  void emitDidUpdate();
  void destroyMarker(unsigned);
  void didDestroyMarker(DisplayMarker *);
};

DisplayMarkerLayer::FindParam startBufferPosition(Point);
DisplayMarkerLayer::FindParam endBufferPosition(Point);
DisplayMarkerLayer::FindParam startScreenPosition(Point);
DisplayMarkerLayer::FindParam endScreenPosition(Point);
DisplayMarkerLayer::FindParam startsInBufferRange(Range);
DisplayMarkerLayer::FindParam endsInBufferRange(Range);
DisplayMarkerLayer::FindParam startsInScreenRange(Range);
DisplayMarkerLayer::FindParam endsInScreenRange(Range);
DisplayMarkerLayer::FindParam startBufferRow(double);
DisplayMarkerLayer::FindParam endBufferRow(double);
DisplayMarkerLayer::FindParam startScreenRow(double);
DisplayMarkerLayer::FindParam endScreenRow(double);
DisplayMarkerLayer::FindParam intersectsBufferRowRange(std::pair<double, double>);
DisplayMarkerLayer::FindParam intersectsScreenRowRange(std::pair<double, double>);
DisplayMarkerLayer::FindParam containsBufferRange(Range);
DisplayMarkerLayer::FindParam containsScreenRange(Range);
DisplayMarkerLayer::FindParam containsBufferPosition(Point);
DisplayMarkerLayer::FindParam containsScreenPosition(Point);
DisplayMarkerLayer::FindParam containedInBufferRange(Range);
DisplayMarkerLayer::FindParam containedInScreenRange(Range);
DisplayMarkerLayer::FindParam intersectsBufferRange(Range);
DisplayMarkerLayer::FindParam intersectsScreenRange(Range);

#endif // DISPLAY_MARKER_LAYER_H_
