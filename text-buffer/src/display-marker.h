#ifndef DISPLAY_MARKER_H_
#define DISPLAY_MARKER_H_

#include "range.h"
#include "event-kit.h"
#include "display-layer.h"

struct DisplayMarkerLayer;
struct Marker;

struct DisplayMarker {
  DisplayMarkerLayer *layer;
  Marker *bufferMarker;
  bool hasChangeObservers;
  Emitter<> didChangeEmitter;
  Emitter<> didDestroyEmitter;
  unsigned id;

  DisplayMarker(DisplayMarkerLayer *, Marker *);
  ~DisplayMarker();
  void destroy();
  void didDestroyBufferMarker();
  void onDidChange(std::function<void()>);
  void onDidDestroy(std::function<void()>);

  bool isReversed();
  int compare(DisplayMarker *);
  bool isEqual(DisplayMarker *);
  Range getBufferRange();
  Range getScreenRange();
  bool setBufferRange(const Range &, optional<bool> = {});
  bool setScreenRange(const Range &, optional<bool> = {});
  Point getHeadBufferPosition();
  bool setHeadBufferPosition(const Point &);
  Point getHeadScreenPosition();
  bool setHeadScreenPosition(const Point &, DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest);
  Point getTailBufferPosition();
  bool setTailBufferPosition(const Point &);
  Point getTailScreenPosition();
  bool setTailScreenPosition(const Point &);
  Point getStartBufferPosition();
  Point getStartScreenPosition();
  Point getEndBufferPosition();
  Point getEndScreenPosition();
  bool hasTail();
  bool plantTail();
  bool clearTail();
  void notifyObservers();
};

#endif // DISPLAY_MARKER_H_
