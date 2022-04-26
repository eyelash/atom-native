#ifndef POINT_H_
#define POINT_H_

#include <cmath>
#include <native-point.h>

struct Point {
  double row;
  double column;

  static uint32_t number_from_js(double number) {
    if (number > 0 && !std::isfinite(number)) {
      return UINT32_MAX;
    } else {
      return std::max(0.0, number);
    }
  }

  Point(double row = 0.0, double column = 0.0): row(row), column(column) {}
  Point(const NativePoint& point): row(point.row), column(point.column) {}

  operator NativePoint() const {
    return NativePoint(number_from_js(row), number_from_js(column));
  }

  Point negate() const {
    return Point(-this->row, -this->column);
  }

  int compare(const Point &other) const {
    if (this->row > other.row) {
      return 1;
    } else if (this->row < other.row) {
      return -1;
    } else {
      if (this->column > other.column) {
        return 1;
      } else if (this->column < other.column) {
        return -1;
      } else {
        return 0;
      }
    }
  }
};

#endif  // POINT_H_
