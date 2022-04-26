#include "range.h"

Range Range::all_inclusive() {
  return Range{NativePoint(), NativePoint::max()};
}

NativePoint Range::extent() const {
  return end.traversal(start);
}
