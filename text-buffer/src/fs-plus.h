#ifndef FS_PLUS_H_
#define FS_PLUS_H_

#include <string>

namespace fsPlus {
  std::string getHomeDirectory();
  std::string normalize(const std::string &);
  std::string resolveHome(const std::string &);
  std::string tildify(const std::string &);
}

#endif // FS_PLUS_H_
