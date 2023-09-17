#include "point.h"
#include <cmath>

static uint32_t number_from_js(double number) {
  if (number > 0 && !std::isfinite(number)) {
    return UINT32_MAX;
  } else {
    return std::max(0.0, number);
  }
}

Point Point::min(const Point &point1, const Point &point2) {
  if (point1.isLessThanOrEqual(point2)) {
    return point1;
  } else {
    return point2;
  }
}

Point Point::max(const Point &point1, const Point &point2) {
  if (point1.compare(point2) >= 0) {
    return point1;
  } else {
    return point2;
  }
}

Point::Point(double row, double column): row(row), column(column) {}
Point::Point(const NativePoint &point): row(point.row), column(point.column) {}

Point::operator NativePoint() const {
  return NativePoint(number_from_js(row), number_from_js(column));
}

Point Point::negate() const {
  return Point(-this->row, -this->column);
}

Point Point::translate(const Point &other) const {
  double row = other.row, column = other.column;
  return Point(this->row + row, this->column + column);
}

Point Point::traverse(const Point &other) const {
  double row, column;
  row = this->row + other.row;
  if (other.row == 0) {
    column = this->column + other.column;
  } else {
    column = other.column;
  }
  return Point(row, column);
}

Point Point::traversalFrom(const Point &other) const {
  if (this->row == other.row) {
    if (this->column == INFINITY && other.column == INFINITY) {
      return Point(0, 0);
    } else {
      return Point(0, this->column - other.column);
    }
  } else {
    return Point(this->row - other.row, this->column);
  }
}

std::pair<Point, Point> Point::splitAt(double column) const {
  double rightColumn;
  if (this->row == 0) {
    rightColumn = this->column - column;
  } else {
    rightColumn = this->column;
  }
  return std::make_pair(Point(0, column), Point(this->row, rightColumn));
}

int Point::compare(const Point &other) const {
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

bool Point::isEqual(const Point &other) const {
  return this->row == other.row && this->column == other.column;
}

bool Point::isLessThan(const Point &other) const {
  return this->compare(other) < 0;
}

bool Point::isLessThanOrEqual(const Point &other) const {
  return this->compare(other) <= 0;
}

bool Point::isGreaterThan(const Point &other) const {
  return this->compare(other) > 0;
}

bool Point::isGreaterThanOrEqual(const Point &other) const {
  return this->compare(other) >= 0;
}

bool Point::operator==(const Point &other) const {
  return this->row == other.row && this->column == other.column;
}

bool Point::operator<(const Point &other) const {
  return this->compare(other) < 0;
}

bool Point::operator<=(const Point &other) const {
  return this->compare(other) <= 0;
}

bool Point::operator>(const Point &other) const {
  return this->compare(other) > 0;
}

bool Point::operator>=(const Point &other) const {
  return this->compare(other) >= 0;
}

bool Point::isZero() const {
  return this->row == 0 && this->column == 0;
}

bool Point::isPositive() const {
  if (this->row > 0) {
    return true;
  } else if (this->row < 0) {
    return false;
  } else {
    return this->column > 0;
  }
}

bool Point::isNegative() const {
  if (this->row < 0) {
    return true;
  } else if (this->row > 0) {
    return false;
  } else {
    return this->column < 0;
  }
}

std::ostream &operator<<(std::ostream &stream, const Point &point) {
  return stream << "(" << point.row << ", " << point.column << ")";
}

const Point Point::ZERO = Point(0, 0);

const Point Point::INFINITY_ = Point(INFINITY, INFINITY);
