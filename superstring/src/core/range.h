#ifndef RANGE_H_
#define RANGE_H_

#include <ostream>
#include "native-point.h"

struct Range {
  NativePoint start;
  NativePoint end;

  static Range all_inclusive();

  NativePoint extent() const;

  bool operator==(const Range &other) const {
    return start == other.start && end == other.end;
  }
};

inline std::ostream &operator<<(std::ostream &stream, const Range &range) {
  return stream << "(" << range.start << ", " << range.end << ")";
}

#endif // RANGE_H_
