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
