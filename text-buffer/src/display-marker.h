#ifndef DISPLAY_MARKER_H_
#define DISPLAY_MARKER_H_

#include "range.h"
#include "event-kit.h"
#include "display-layer.h"

class DisplayMarkerLayer;
class Marker;

class DisplayMarker {
public:
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

  bool isReversed() const;
  int compare(const DisplayMarker *) const;
  bool isEqual(const DisplayMarker *) const;
  Range getBufferRange() const;
  Range getScreenRange();
  bool setBufferRange(const Range &, optional<bool> = optional<bool>());
  bool setScreenRange(const Range &, optional<bool> = optional<bool>());
  Point getHeadBufferPosition() const;
  bool setHeadBufferPosition(const Point &);
  Point getHeadScreenPosition();
  bool setHeadScreenPosition(const Point &, DisplayLayer::ClipDirection = DisplayLayer::ClipDirection::closest);
  Point getTailBufferPosition() const;
  bool setTailBufferPosition(const Point &);
  Point getTailScreenPosition();
  bool setTailScreenPosition(const Point &);
  Point getStartBufferPosition() const;
  Point getStartScreenPosition();
  Point getEndBufferPosition() const;
  Point getEndScreenPosition();
  bool hasTail() const;
  bool plantTail();
  bool clearTail();
  void notifyObservers();
};

#endif // DISPLAY_MARKER_H_
