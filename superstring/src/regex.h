#ifndef REGEX_H_
#define REGEX_H_

#include "optional.h"
#include <string>

struct pcre2_real_code_16;
struct pcre2_real_match_data_16;
struct BuildRegexResult;

class Regex {
  pcre2_real_code_16 *code;
  Regex(pcre2_real_code_16 *);

 public:
  Regex();
  Regex(const char16_t *, uint32_t, std::u16string *error_message = nullptr, bool ignore_case = false, bool unicode = false);
  Regex(const std::u16string &, std::u16string *error_message = nullptr, bool ignore_case = false, bool unicode = false);
  Regex(Regex &&);
  ~Regex();
  Regex &operator=(Regex &&);
  operator bool() const;

  struct Range {
    size_t start_offset;
    size_t end_offset;
  };

  class MatchData {
    pcre2_real_match_data_16 *data;
    friend class Regex;

   public:
    MatchData(const Regex &);
    ~MatchData();

    uint32_t size();
    Range operator [](uint32_t);
  };

  struct MatchResult {
    enum {
      None,
      Error,
      Partial,
      Full,
    } type;

    size_t start_offset;
    size_t end_offset;

    operator bool() const;
  };

  enum MatchOptions {
    None = 0,
    IsBeginningOfLine = 1,
    IsEndOfLine = 2,
    IsEndSearch = 4,
  };

  MatchResult match(const char16_t *data, size_t length, MatchData &, unsigned options) const;
  MatchResult match(const std::u16string &, MatchData &, unsigned options = 7) const;
  MatchResult match(const char16_t *, size_t) const;
  MatchResult match(const std::u16string &) const;
  MatchResult match(char16_t) const;
  bool match(const char16_t *, size_t, size_t &last_index) const;
  bool match(const std::u16string &, size_t &last_index) const;
};

struct BuildRegexResult {
  optional<Regex> regex;
  std::u16string error_message;
};

#endif  // REGX_H_
