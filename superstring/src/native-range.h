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

std::ostream &operator<<(std::ostream &, const NativeRange &);

#endif // NATIVE_RANGE_H_
