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

static std::string normalizeString(const std::string &path, bool allowAboveRoot, char separator, bool (*isPathSeparator)(char)) {
  std::string res;
  double lastSegmentLength = 0;
  double lastSlash = -1;
  double dots = 0;
  char code = 0;
  for (double i = 0; i <= path.size(); ++i) {
    if (i < path.size())
      code = path[i];
    else if (isPathSeparator(code))
      break;
    else
      code = '/';

    if (isPathSeparator(code)) {
      if (lastSlash == i - 1 || dots == 1) {
        // NOOP
      } else if (dots == 2) {
        if (res.size() < 2 || lastSegmentLength != 2 ||
            res[res.size() - 1] != '.' ||
            res[res.size() - 2] != '.') {
          if (res.size() > 2) {
            const size_t lastSlashIndex = res.rfind(separator);
            if (lastSlashIndex == std::string::npos) {
              res = "";
              lastSegmentLength = 0;
            } else {
              res = res.substr(0, lastSlashIndex);
              lastSegmentLength =
                res.size() - 1.0 - res.rfind(separator);
            }
            lastSlash = i;
            dots = 0;
            continue;
          } else if (res.size() != 0) {
            res = "";
            lastSegmentLength = 0;
            lastSlash = i;
            dots = 0;
            continue;
          }
        }
        if (allowAboveRoot) {
          if (res.size() > 0) {
            res += separator;
          }
          res += "..";
          lastSegmentLength = 2;
        }
      } else {
        if (res.size() > 0) {
          res += separator;
          res += path.substr(lastSlash + 1, i - lastSlash - 1);
        } else {
          res = path.substr(lastSlash + 1, i - lastSlash - 1);
        }
        lastSegmentLength = i - lastSlash - 1;
      }
      lastSlash = i;
      dots = 0;
    } else if (code == '.' && dots != -1) {
      ++dots;
    } else {
      dots = -1;
    }
  }
  return res;
}

std::string path::normalize(std::string path) {
  if (path.size() == 0)
    return ".";

  const bool isAbsolute = path[0] == '/';
  const bool trailingSeparator = path[path.size() - 1] == '/';

  // Normalize the path
  path = normalizeString(path, !isAbsolute, '/', isPathSeparator);

  if (path.size() == 0) {
    if (isAbsolute)
      return "/";
    return trailingSeparator ? "./" : ".";
  }
  if (trailingSeparator)
    path += '/';

  return isAbsolute ? "/" + path : path;
}

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

#ifdef WIN32
const char path::sep = '\\';
#else
const char path::sep = '/';
#endif
