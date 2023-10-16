#include "helpers.h"
#include <regex.h>

static const Regex LF_REGEX(u"\\n");

Point extentForText(const std::u16string &text) {
  size_t lastLineStartIndex = 0;
  double row = 0;
  size_t lastIndex = 0;
  while (LF_REGEX.match(text, lastIndex)) {
    row++;
    lastLineStartIndex = lastIndex;
  }
  return {row, static_cast<double>(text.size() - lastLineStartIndex)};
}

std::u16string toUpperCase(std::u16string string) {
  // this only works for ASCII strings
  for (char16_t &c : string) {
    if (c >= u'a' && c <= u'z') {
      c = u'A' + (c - u'a');
    }
  }
  return string;
}

std::u16string toLowerCase(std::u16string string) {
  // this only works for ASCII strings
  for (char16_t &c : string) {
    if (c >= u'A' && c <= u'Z') {
      c = u'a' + (c - u'A');
    }
  }
  return string;
}

std::u16string escapeRegExp(const std::u16string &string) {
  std::u16string result;
  for (char16_t c : string) {
    if (std::u16string(u"-/\\^$*+?.()|[]{}").find(c) != std::u16string::npos) {
      result.push_back(u'\\');
    }
    result.push_back(c);
  }
  return result;
}
