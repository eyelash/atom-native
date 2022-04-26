#ifndef FLAT_TEXT_SLICE_H_
#define FLAT_TEXT_SLICE_H_

#include <vector>
#include "native-point.h"
#include "range.h"
#include "text.h"

class TextSlice {
 public:
  const Text *text;
  NativePoint start_position;
  NativePoint end_position;

  TextSlice(const Text *text, NativePoint start_position, NativePoint end_position);
  size_t start_offset() const;
  size_t end_offset() const;

  TextSlice();
  TextSlice(const Text &text);
  std::pair<TextSlice, TextSlice> split(NativePoint) const;
  std::pair<TextSlice, TextSlice> split(uint32_t) const;
  TextSlice prefix(NativePoint) const;
  TextSlice prefix(uint32_t) const;
  TextSlice suffix(NativePoint) const;
  TextSlice slice(Range range) const;
  NativePoint position_for_offset(uint32_t offset, uint32_t min_row = 0) const;
  NativePoint extent() const;
  uint16_t front() const;
  uint16_t back() const;
  bool is_valid() const;

  const char16_t *data() const;
  uint32_t size() const;
  bool empty() const;

  Text::const_iterator begin() const;
  Text::const_iterator end() const;
};

#endif // FLAT_TEXT_SLICE_H_
