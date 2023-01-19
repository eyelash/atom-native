#include "file.h"

File::File() {}

File::File(const std::string &path) : path(path) {}

const std::string &File::getPath() const {
  return path;
}
