#ifndef POINT_HELPERS_H_
#define POINT_HELPERS_H_

#include "point.h"

inline Point traverse(Point start, Point distance) {
  if (distance.row == 0) {
    return Point{start.row, start.column + distance.column};
  } else {
    return Point{start.row + distance.row, distance.column};
  }
}

inline Point traversal(Point end, Point start) {
  if (end.row == start.row) {
    return Point{0, end.column - start.column};
  } else {
    return Point{end.row - start.row, end.column};
  }
}

#endif  // POINT_HELPERS_H_
