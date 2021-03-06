#ifndef RANGE_H_
#define RANGE_H_

#include "point.h"
#include <native-range.h>

struct Range {
  Point start;
  Point end;

  Range(const Point &pointA = Point(0, 0), const Point &pointB = Point(0, 0));
  Range(const NativeRange &);

  operator NativeRange() const;
  Range negate() const;
  bool isEmpty() const;
  bool isSingleLine() const;
  double getRowCount() const;
  Range union_(const Range &) const;
  Range translate(const Point &, const Point &) const;
  Range translate(const Point &) const;
  Range traverse(const Point &) const;
  int compare(const Range &) const;
  bool isEqual(const Range &) const;
  bool coversSameRows(const Range &) const;
  bool intersectsWith(const Range &, bool = false) const;
  bool containsRange(const Range &, bool) const;
  bool containsPoint(const Point &, bool) const;
  bool intersectsRow(double) const;
  bool intersectsRowRange(double, double) const;
  Point getExtent() const;
  Point toDelta() const;
};

#endif  // RANGE_H_
