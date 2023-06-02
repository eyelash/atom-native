#include "path.h"

#ifdef WIN32

static bool isPathSeparator(char c) {
  return c == '/' || c == '\\';
}

#else

static bool isPathSeparator(char c) {
  return c == '/';
}

#endif

std::string path::basename(const std::string &path) {
  double start = 0;
  double end = -1;
  bool matchedSlash = true;

  for (double i = path.size() - 1.0; i >= start; --i) {
    if (isPathSeparator(path[i])) {
      // If we reached a path separator that was not part of a set of path
      // separators at the end of the string, stop now
      if (!matchedSlash) {
        start = i + 1;
        break;
      }
    } else if (end == -1) {
      // We saw the first non-path separator, mark this as the end of our
      // path component
      matchedSlash = false;
      end = i + 1;
    }
  }

  if (end == -1)
    return "";
  return path.substr(start, end - start);
}
