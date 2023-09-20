#include "range.h"

Range Range::fromPointWithDelta(const Point &startPoint, double rowDelta, double columnDelta) {
  const Point endPoint = Point(startPoint.row + rowDelta, startPoint.column + columnDelta);
  return Range(startPoint, endPoint);
}

Range Range::fromPointWithTraversalExtent(const Point &startPoint, const Point &extent) {
  return Range(startPoint, startPoint.traverse(extent));
}

Range::Range(const Point &pointA, const Point &pointB) {
  if (pointA.isLessThanOrEqual(pointB)) {
    this->start = pointA;
    this->end = pointB;
  } else {
    this->start = pointB;
    this->end = pointA;
  }
}

Range::Range(const NativeRange &range): start(range.start), end(range.end) {}

Range::operator NativeRange() const {
  return NativeRange{start, end};
}

Range Range::negate() const {
  return Range(this->start.negate(), this->end.negate());
}

bool Range::isEmpty() const {
  return this->start.isEqual(this->end);
}

bool Range::isSingleLine() const {
  return this->start.row == this->end.row;
}

double Range::getRowCount() const {
  return this->end.row - this->start.row + 1;
}

Range Range::union_(const Range &otherRange) const {
  const Point start = this->start.isLessThan(otherRange.start) ? this->start : otherRange.start;
  const Point end = this->end.isGreaterThan(otherRange.end) ? this->end : otherRange.end;
  return Range(start, end);
}

Range Range::translate(const Point &startDelta, const Point &endDelta) const {
  return Range(this->start.translate(startDelta), this->end.translate(endDelta));
}

Range Range::translate(const Point &startDelta) const {
  return translate(startDelta, startDelta);
}

Range Range::traverse(const Point &delta) const {
  return Range(this->start.traverse(delta), this->end.traverse(delta));
}

int Range::compare(const Range &other) const {
  if (int value = this->start.compare(other.start)) {
    return value;
  } else {
    return other.end.compare(this->end);
  }
}

bool Range::isEqual(const Range &other) const {
  return other.start.isEqual(this->start) && other.end.isEqual(this->end);
}

bool Range::operator==(const Range &other) const {
  return other.start.isEqual(this->start) && other.end.isEqual(this->end);
}

bool Range::coversSameRows(const Range &other) const {
  return this->start.row == other.start.row && this->end.row == other.end.row;
}

bool Range::intersectsWith(const Range &otherRange, bool exclusive) const {
  if (exclusive) {
    return !(this->end.isLessThanOrEqual(otherRange.start) || this->start.isGreaterThanOrEqual(otherRange.end));
  } else {
    return !(this->end.isLessThan(otherRange.start) || this->start.isGreaterThan(otherRange.end));
  }
}

bool Range::containsRange(const Range &otherRange, bool exclusive) const {
  const Point &start = otherRange.start;
  const Point &end = otherRange.end;
  return this->containsPoint(start, exclusive) && this->containsPoint(end, exclusive);
}

bool Range::containsPoint(const Point &point, bool exclusive) const {
  if (exclusive) {
    return point.isGreaterThan(this->start) && point.isLessThan(this->end);
  } else {
    return point.isGreaterThanOrEqual(this->start) && point.isLessThanOrEqual(this->end);
  }
}

bool Range::intersectsRow(double row) const {
  return (this->start.row <= row && row <= this->end.row);
}

bool Range::intersectsRowRange(double startRow, double endRow) const {
  if (startRow > endRow) {
    std::swap(startRow, endRow);
  }
  return this->end.row >= startRow && endRow >= this->start.row;
}

Point Range::getExtent() const {
  return this->end.traversalFrom(this->start);
}

Point Range::toDelta() const {
  double rows, columns;
  rows = this->end.row - this->start.row;
  if (rows == 0) {
    columns = this->end.column - this->start.column;
  } else {
    columns = this->end.column;
  }
  return Point(rows, columns);
}

std::ostream &operator<<(std::ostream &stream, const Range &range) {
  return stream << "[" << range.start << " - " << range.end << "]";
}
