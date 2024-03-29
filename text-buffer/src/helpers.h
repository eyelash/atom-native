#ifndef HELPERS_H_
#define HELPERS_H_

#include "point.h"
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>

template <typename T> void spliceArray(std::vector<T> &array, double start, double removedCount, const std::vector<T> &insertedItems = std::vector<T>()) {
  const double oldLength = array.size();
  const double insertedCount = insertedItems.size();
  removedCount = std::min(removedCount, oldLength - start);
  const double lengthDelta = insertedCount - removedCount;
  const double newLength = oldLength + lengthDelta;

  if (lengthDelta > 0) {
    array.resize(newLength);
    for (double i = newLength - 1, end = start + insertedCount; i >= end; i--) {
      array[i] = std::move(array[i - lengthDelta]);
    }
  } else if (lengthDelta < 0) {
    for (double i = start + insertedCount, end = newLength; i < end; i++) {
      array[i] = std::move(array[i - lengthDelta]);
    }
    array.resize(newLength);
  }

  for (double i = 0; i < insertedCount; i++) {
    array[start + i] = insertedItems[i];
  }
}

template <typename T> std::string toString(const T &t) {
  std::ostringstream stream;
  stream << t;
  return stream.str();
}

template <typename T, typename C> std::vector<T> sort(std::vector<T> &&v, C comp) {
  std::sort(v.begin(), v.end(), comp);
  return v;
}

Point extentForText(const std::u16string &);

template <typename T> class Slice {
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

std::u16string toUpperCase(std::u16string);
std::u16string toLowerCase(std::u16string);

template <typename CharT> std::basic_string<CharT> join(const std::vector<std::basic_string<CharT>> &vector, const CharT *separator, size_t separator_length) {
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
template <typename CharT> std::basic_string<CharT> join(const std::vector<std::basic_string<CharT>> &vector, const CharT *separator) {
  return join(vector, separator, std::char_traits<CharT>::length(separator));
}
template <typename CharT> std::basic_string<CharT> join(const std::vector<std::basic_string<CharT>> &vector, const CharT separator) {
  return join(vector, &separator, 1);
}

template <typename CharT> std::vector<std::basic_string<CharT>> split(const std::basic_string<CharT> &string, const CharT *separator, size_t separator_length) {
  std::vector<std::basic_string<CharT>> result;
  size_t position = 0;
  for (size_t i; (i = string.find(separator, position, separator_length)) != std::basic_string<CharT>::npos; position = i + separator_length) {
    result.push_back(string.substr(position, i - position));
  }
  result.push_back(string.substr(position));
  return result;
}
template <typename CharT> std::vector<std::basic_string<CharT>> split(const std::basic_string<CharT> &string, CharT separator) {
  return split(string, &separator, 1);
}

std::u16string escapeRegExp(const std::u16string &string);

template <typename K, typename V> V *get(const std::unordered_map<K, V *> &m, const K &k) {
  auto i = m.find(k);
  if (i != m.end()) {
    return i->second;
  } else {
    return nullptr;
  }
}

template <typename T> T pop(std::vector<T> &v) {
  T result = std::move(v.back());
  v.pop_back();
  return result;
}

template <typename T> T shift(std::vector<T> &v) {
  T result = std::move(v.front());
  v.erase(v.begin());
  return result;
}

template <typename K, typename V> std::vector<K> keys(const std::unordered_map<K, V> &map) {
  std::vector<K> result;
  for (const auto &entry : map) {
    result.push_back(entry.first);
  }
  return result;
}

#endif // HELPERS_H_
