#include "native-range.h"

NativeRange NativeRange::all_inclusive() {
  return NativeRange{NativePoint(), NativePoint::max()};
}

NativePoint NativeRange::extent() const {
  return end.traversal(start);
}
