#ifndef NATIVE_POINT_H_
#define NATIVE_POINT_H_

#include <ostream>
#include "serializer.h"

struct NativePoint {
  unsigned row;
  unsigned column;

  static NativePoint min(const NativePoint &left, const NativePoint &right);
  static NativePoint max(const NativePoint &left, const NativePoint &right);
  static NativePoint max();

  NativePoint();
  NativePoint(unsigned row, unsigned column);
  NativePoint(Deserializer &input);

  int compare(const NativePoint &other) const;
  bool is_zero() const;
  NativePoint traverse(const NativePoint &other) const;
  NativePoint traversal(const NativePoint &other) const;
  void serialize(Serializer &output) const;

  bool operator!=(const NativePoint &other) const;
  bool operator==(const NativePoint &other) const;
  bool operator<(const NativePoint &other) const;
  bool operator<=(const NativePoint &other) const;
  bool operator>(const NativePoint &other) const;
  bool operator>=(const NativePoint &other) const;
};

std::ostream &operator<<(std::ostream &, const NativePoint &);

#endif // NATIVE_POINT_H_
