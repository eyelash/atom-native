#ifndef POINT_H_
#define POINT_H_

#include <native-point.h>

struct Point {
  double row;
  double column;

  static Point min(const Point &, const Point &);
  static Point max(const Point &, const Point &);

  Point(double row = 0.0, double column = 0.0);
  Point(const NativePoint &);

  operator NativePoint() const;
  Point negate() const;
  Point translate(const Point &) const;
  Point traverse(const Point &) const;
  Point traversalFrom(const Point &) const;
  std::pair<Point, Point> splitAt(double) const;
  int compare(const Point &) const;
  bool isEqual(const Point &) const;
  bool isLessThan(const Point &) const;
  bool isLessThanOrEqual(const Point &) const;
  bool isGreaterThan(const Point &) const;
  bool isGreaterThanOrEqual(const Point &) const;
  bool operator==(const Point &) const;
  bool operator<(const Point &) const;
  bool operator<=(const Point &) const;
  bool operator>(const Point &) const;
  bool operator>=(const Point &) const;
  bool isZero() const;
  bool isPositive() const;
  bool isNegative() const;

  static const Point ZERO;
  static const Point INFINITY_;
};

std::ostream &operator<<(std::ostream &, const Point &);

#endif // POINT_H_
