#ifndef MARKER_H_
#define MARKER_H_

#include "range.h"
#include "event-kit.h"
#include <optional.h>

struct MarkerLayer;

struct Marker {
  enum struct InvalidationStrategy {
    never,
    surround,
    overlap,
    inside,
    touch
  };
  struct Snapshot {
    Range range;
    bool reversed;
    bool tailed;
    InvalidationStrategy invalidate;
    optional<bool> exclusive;
  };
  struct Params {
    optional<Range> range;
    optional<bool> reversed;
    optional<bool> tailed;
  };

  unsigned id;
  MarkerLayer *layer;
  Emitter<> didDestroyEmitter;
  Emitter<> didChangeEmitter;
  bool tailed;
  bool reversed;
  InvalidationStrategy invalidate;
  bool hasChangeObservers;
  optional<bool> exclusive;
  Snapshot previousEventState;

  Marker(unsigned, MarkerLayer *, const Range &, const Params & = {}, bool = false);
  ~Marker();

  void onDidDestroy(std::function<void()>);
  void onDidChange(std::function<void()>);
  Range getRange();
  bool setRange(const Range &, optional<bool> = {});
  Point getHeadPosition();
  bool setHeadPosition(const Point &);
  Point getTailPosition();
  bool setTailPosition(const Point &);
  Point getStartPosition();
  Point getEndPosition();
  bool clearTail();
  bool plantTail();
  bool isReversed();
  bool hasTail();
  bool isExclusive();
  bool isEqual(Marker *);
  InvalidationStrategy getInvalidationStrategy();
  void destroy(bool = false);
  int compare(Marker *);
  bool update(const Range &, const Params &, bool = false, bool = false);
  Snapshot getSnapshot(Range, bool = true);
  void emitChangeEvent(Range, bool, bool);
};

#endif // MARKER_H_
