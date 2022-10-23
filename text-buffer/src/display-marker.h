#ifndef DISPLAY_MARKER_H_
#define DISPLAY_MARKER_H_

#include "range.h"
#include "event-kit.h"

class DisplayMarkerLayer;
class Marker;

class DisplayMarker {
public:
  DisplayMarkerLayer *layer;
  Marker *bufferMarker;
  Emitter<> didDestroyEmitter;
  unsigned id;

  DisplayMarker(DisplayMarkerLayer *, Marker *);
  ~DisplayMarker();
  void destroy();
  void didDestroyBufferMarker();
  void onDidDestroy(std::function<void()>);

  bool isReversed() const;
  int compare(const DisplayMarker *) const;
  bool isEqual(const DisplayMarker *) const;
  Range getBufferRange() const;
  Range getScreenRange();
  bool setBufferRange(const Range &);
  bool setScreenRange(const Range &);
  Point getHeadBufferPosition() const;
  bool setHeadBufferPosition(const Point &);
  Point getHeadScreenPosition();
  bool setHeadScreenPosition(const Point &);
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
};

#endif  // DISPLAY_MARKER_H_
