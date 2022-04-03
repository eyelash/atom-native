#ifndef HELPERS_H_
#define HELPERS_H_

#include <regex.h>
#include <point.h>

inline Point extentForText(const std::u16string &text) {
  static const Regex LF_REGEX(u"\\n", nullptr);
  size_t lastLineStartIndex = 0;
  uint32_t row = 0;
  Regex::MatchData match_data(LF_REGEX);
  Regex::MatchResult result = LF_REGEX.match(text.data() + lastLineStartIndex, text.size() - lastLineStartIndex, match_data);
  while (result.type == Regex::MatchResult::Partial || result.type == Regex::MatchResult::Full) {
    row++;
    lastLineStartIndex = result.end_offset;
    result = LF_REGEX.match(text.data() + lastLineStartIndex, text.size() - lastLineStartIndex, match_data);
  }
  return {row, static_cast<unsigned>(text.size() - lastLineStartIndex)};
}

#endif  // HELPERS_H_
