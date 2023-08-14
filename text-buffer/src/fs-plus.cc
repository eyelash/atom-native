#include "fs-plus.h"
#include "path.h"

std::string fsPlus::getHomeDirectory() {
  const char *homeDirectory = getenv("HOME");
#ifdef WIN32
  if (homeDirectory == nullptr) {
    homeDirectory = getenv("USERPROFILE");
  }
#endif
  return homeDirectory ? homeDirectory : std::string();
}

std::string fsPlus::normalize(const std::string &pathToNormalize) {
  return fsPlus::resolveHome(path::normalize(pathToNormalize));
}

std::string fsPlus::resolveHome(const std::string &relativePath) {
  if (relativePath == "~") {
    return fsPlus::getHomeDirectory();
  } else if (relativePath.find({'~', '/'}) == 0) {
    return fsPlus::getHomeDirectory() + relativePath.substr(1);
  }
  return relativePath;
}

static bool startsWith(const std::string &s, const std::string &prefix) {
  if (s.size() < prefix.size()) {
    return false;
  }
  for (size_t i = 0; i < prefix.size(); i++) {
    if (s[i] != prefix[i]) return false;
  }
  return true;
}

std::string fsPlus::tildify(const std::string &pathToTildify) {
#ifdef WIN32
  return pathToTildify;
#else
  const std::string normalized = fsPlus::normalize(pathToTildify);
  const std::string homeDir = fsPlus::getHomeDirectory();
  if (homeDir.empty()) {
    return pathToTildify;
  }
  if (normalized == homeDir) {
    return "~";
  }
  if (!startsWith(normalized, path::join(homeDir, path::sep))) {
    return pathToTildify;
  }
  return path::join('~', path::sep, normalized.substr(homeDir.size() + 1));
#endif
}
