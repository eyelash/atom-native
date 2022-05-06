#ifndef HELPERS_H_
#define HELPERS_H_

#include "point.h"
#include <vector>
#include <algorithm>
#include <iterator>

template <class T> void spliceArray(std::vector<T>& array, double start, double removedCount, const std::vector<T>& insertedItems = std::vector<T>()) {
  const double oldLength = array.size();
  const double insertedCount = insertedItems.size();
  removedCount = std::min(removedCount, oldLength - start);
  const double lengthDelta = insertedCount - removedCount;
  const double newLength = oldLength + lengthDelta;

  if (lengthDelta > 0) {
    array.resize(newLength);
    for (double i = newLength - 1, end = start + insertedCount; i >= end; i--) {
      array[i] = array[i - lengthDelta];
    }
  } else if (lengthDelta < 0) {
    for (double i = start + insertedCount, end = newLength; i < end; i++) {
      array[i] = array[i - lengthDelta];
    }
    array.resize(newLength);
  }

  for (double i = 0; i < insertedCount; i++) {
    array[start + i] = insertedItems[i];
  }
}

template <class T, class C> std::vector<T> sort(std::vector<T> &&v, C comp) {
  std::sort(v.begin(), v.end(), comp);
  return v;
}

Point extentForText(const std::u16string &);

#endif  // HELPERS_H_
