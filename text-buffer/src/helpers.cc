#include "helpers.h"
#include <regex.h>

static const Regex LF_REGEX(u"\\n", nullptr);

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

bool includes(const char16_t *string, char16_t character) {
  const size_t length = std::char_traits<char16_t>::length(string);
  return std::char_traits<char16_t>::find(string, length, character) != nullptr;
}

std::u16string escapeRegExp(const std::u16string &string) {
  std::u16string result;
  for (char16_t c : string) {
    if (includes(u"-/\\^$*+?.()|[]{}", c)) {
      result.push_back(u'\\');
    }
    result.push_back(c);
  }
  return result;
}
