#ifndef MARKER_H_
#define MARKER_H_

#include "range.h"
#include <optional.h>

class MarkerLayer;

class Marker {
  unsigned id;
  MarkerLayer *layer;
  bool tailed;
  bool reversed;

public:
  Marker(unsigned, MarkerLayer *, Range);
  ~Marker();

  Range getRange();
  bool setRange(const Range &, optional<bool>);
  Point getHeadPosition();
  bool setHeadPosition(const Point &);
  Point getTailPosition();
  bool setTailPosition(const Point &);
  Point getStartPosition();
  Point getEndPosition();
  bool clearTail();
  bool plantTail();
  bool isReversed() const;
  bool hasTail() const;

private:
  struct UpdateParams {
    optional<Range> range;
    optional<bool> reversed;
    optional<bool> tailed;
  };
  bool update(Range, const UpdateParams &);
};

#endif  // MARKER_H_
