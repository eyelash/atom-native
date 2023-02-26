#ifndef MARKER_H_
#define MARKER_H_

#include "range.h"
#include <optional.h>

class MarkerLayer;

class Marker {
public:
  enum class InvalidationStrategy {
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

private:
  MarkerLayer *layer;
  bool tailed;
  bool reversed;
  InvalidationStrategy invalidate;
  optional<bool> exclusive;

public:
  unsigned id;

  Marker(unsigned, MarkerLayer *, Range, const Params & = Params{}, bool = false);
  ~Marker();

  Range getRange() const;
  bool setRange(const Range &, optional<bool> = optional<bool>());
  Point getHeadPosition() const;
  bool setHeadPosition(const Point &);
  Point getTailPosition() const;
  bool setTailPosition(const Point &);
  Point getStartPosition() const;
  Point getEndPosition() const;
  bool clearTail();
  bool plantTail();
  bool isReversed() const;
  bool hasTail() const;
  bool isExclusive();
  bool isEqual(const Marker *) const;
  InvalidationStrategy getInvalidationStrategy() const;
  void destroy(bool = false);
  int compare(const Marker *) const;
  bool update(const Range &, const Params &, bool = false, bool = false);
  Snapshot getSnapshot(Range, bool = true);
};

#endif // MARKER_H_
