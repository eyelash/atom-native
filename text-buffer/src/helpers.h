#ifndef HELPERS_H_
#define HELPERS_H_

#include "point.h"
#include <vector>
#include <algorithm>
#include <iterator>

template <class T> void spliceArray(std::vector<T> &array, double start, double removedCount, const std::vector<T> &insertedItems = std::vector<T>()) {
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

template <class T> class Slice {
  const T *data_;
  std::size_t size_;
public:
  Slice(std::initializer_list<T> l): data_(l.begin()), size_(l.size()) {}
  Slice(const std::vector<T> &v): data_(v.data()), size_(v.size()) {}
  const T &operator [](std::size_t i) const {
    return data_[i];
  }
  std::size_t size() const {
    return size_;
  }
  const T *begin() const {
    return data_;
  }
  const T *end() const {
    return data_ + size_;
  }
};

template <class CharT> bool includes(const std::basic_string<CharT> &haystack, const CharT *needle, size_t needle_length, size_t position = 0) {
  return haystack.find(needle, position, needle_length) != std::basic_string<CharT>::npos;
}
template <class CharT> bool includes(const std::basic_string<CharT> &haystack, CharT needle, size_t position = 0) {
  return includes(haystack, &needle, 1, position);
}

std::u16string toUpperCase(std::u16string);
std::u16string toLowerCase(std::u16string);

template <class CharT> std::basic_string<CharT> join(const std::vector<std::basic_string<CharT>> &vector, const CharT *separator, size_t separator_length) {
  std::basic_string<CharT> result;
  auto iterator = vector.begin();
  if (iterator != vector.end()) {
    result.append(*iterator);
    ++iterator;
    while (iterator != vector.end()) {
      result.append(separator, separator_length);
      result.append(*iterator);
      ++iterator;
    }
  }
  return result;
}
template <class CharT> std::basic_string<CharT> join(const std::vector<std::basic_string<CharT>> &vector, const CharT *separator) {
  return join(vector, separator, std::char_traits<CharT>::length(separator));
}

template <class CharT> std::vector<std::basic_string<CharT>> split(const std::basic_string<CharT> &string, const CharT *separator, size_t separator_length) {
  std::vector<std::basic_string<CharT>> result;
  size_t position = 0;
  for (size_t i; (i = string.find(separator, position, separator_length)) != std::basic_string<CharT>::npos; position = i + separator_length) {
    result.push_back(string.substr(position, i - position));
  }
  result.push_back(string.substr(position));
  return result;
}

std::u16string escapeRegExp(const std::u16string &string);

template <class T> T pop(std::vector<T> &v) {
  T result = std::move(v.back());
  v.pop_back();
  return result;
}

template <class T> T shift(std::vector<T> &v) {
  T result = std::move(v.front());
  v.erase(v.begin());
  return result;
}

#endif // HELPERS_H_
