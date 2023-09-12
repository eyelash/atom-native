#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <vector>
#include <string>

struct LanguageMode;
struct TextBuffer;

struct Grammar {
  const char *name;
  const char *scopeName;
  std::vector<std::string> fileTypes;

  Grammar(const char *, const char *);
  virtual ~Grammar();

  void addFileTypes(const char *);
  template <typename T0, typename T1, typename... T> void addFileTypes(T0&& t0, T1&& t1, T&&... t) {
    addFileTypes(std::forward<T0>(t0));
    addFileTypes(std::forward<T1>(t1), std::forward<T>(t)...);
  }
  template <typename... T> void setFileTypes(T&&... t) {
    addFileTypes(std::forward<T>(t)...);
  }
  virtual LanguageMode *getLanguageMode(TextBuffer *) = 0;
};

#endif // GRAMMAR_H_
