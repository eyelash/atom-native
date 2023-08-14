#ifndef PATH_H_
#define PATH_H_

#include <string>

namespace path {
  std::string normalize(std::string);
  std::string basename(const std::string &);
  extern const char sep;
  inline size_t length_(char) {
    return 1;
  }
  inline size_t length_(const char *s) {
    return std::char_traits<char>::length(s);
  }
  inline size_t length_(const std::string &s) {
    return s.size();
  }
  inline void join_(std::string&) {}
  template <class T0, class... T> void join_(std::string& joined, const T0& arg, const T&... args) {
    if (length_(arg) > 0) {
      if (joined.empty()) {
        joined = arg;
      } else {
        joined += sep;
        joined += arg;
      }
    }
    join_(joined, args...);
  }
  inline std::string join() {
    return ".";
  }
  template <class T0, class... T> std::string join(const T0& arg, const T&... args) {
    std::string joined;
    join_(joined, arg, args...);
    return normalize(std::move(joined));
  }
}

#endif // PATH_H_
