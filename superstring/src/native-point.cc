#include <climits>
#include <algorithm>
#include "native-point.h"

NativePoint NativePoint::min(const NativePoint &left, const NativePoint &right) {
  return left <= right ? left : right;
}

NativePoint NativePoint::max(const NativePoint &left, const NativePoint &right) {
  return left >= right ? left : right;
}

NativePoint NativePoint::max() {
  return NativePoint(UINT32_MAX, UINT32_MAX);
}

NativePoint::NativePoint() : NativePoint(0, 0) {}

NativePoint::NativePoint(uint32_t row, uint32_t column) : row{row}, column{column} {}

NativePoint::NativePoint(Deserializer &input) :
  row{input.read<uint32_t>()},
  column{input.read<uint32_t>()} {}

int NativePoint::compare(const NativePoint &other) const {
  if (row < other.row) return -1;
  if (row > other.row) return 1;
  if (column < other.column) return -1;
  if (column > other.column) return 1;
  return 0;
}

bool NativePoint::is_zero() const {
  return row == 0 && column == 0;
}

static uint32_t checked_add(uint32_t a, uint32_t b) {
  return std::min<uint64_t>(
    UINT32_MAX,
    static_cast<uint64_t>(a) + static_cast<uint64_t>(b)
  );
}

NativePoint NativePoint::traverse(const NativePoint &traversal) const {
  if (traversal.row == 0) {
    return NativePoint(row, checked_add(column, traversal.column));
  } else {
    return NativePoint(checked_add(row, traversal.row), traversal.column);
  }
}

NativePoint NativePoint::traversal(const NativePoint &start) const {
  if (row == start.row) {
    return NativePoint(0, column - start.column);
  } else {
    return NativePoint(row - start.row, column);
  }
}

void NativePoint::serialize(Serializer &output) const {
  output.append(row);
  output.append(column);
}

bool NativePoint::operator==(const NativePoint &other) const {
  return compare(other) == 0;
}

bool NativePoint::operator!=(const NativePoint &other) const {
  return compare(other) != 0;
}

bool NativePoint::operator<(const NativePoint &other) const {
  return compare(other) < 0;
}

bool NativePoint::operator<=(const NativePoint &other) const {
  return compare(other) <= 0;
}

bool NativePoint::operator>(const NativePoint &other) const {
  return compare(other) > 0;
}

bool NativePoint::operator>=(const NativePoint &other) const {
  return compare(other) >= 0;
}

std::ostream &operator<<(std::ostream &stream, const NativePoint &point) {
  return stream << "(" << point.row << ", " << point.column << ")";
}
