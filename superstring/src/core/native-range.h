#ifndef NATIVE_RANGE_H_
#define NATIVE_RANGE_H_

#include <ostream>
#include "native-point.h"

struct NativeRange {
  NativePoint start;
  NativePoint end;

  static NativeRange all_inclusive();

  NativePoint extent() const;

  bool operator==(const NativeRange &other) const {
    return start == other.start && end == other.end;
  }
};

inline std::ostream &operator<<(std::ostream &stream, const NativeRange &range) {
  return stream << "(" << range.start << ", " << range.end << ")";
}

#endif // NATIVE_RANGE_H_
