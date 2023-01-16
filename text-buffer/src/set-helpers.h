#ifndef SET_HELPERS_H_
#define SET_HELPERS_H_

#include <flat_set.h>

template <class T> void intersectSet(flat_set<T> &set, const flat_set<T> &other) {
	for (T value : set) {
    if (!other.count(value)) {
      set.erase(value);
    }
  }
}

#endif // SET_HELPERS_H_
