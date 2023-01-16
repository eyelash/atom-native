#ifndef FILE_H_
#define FILE_H_

#include <string>

class File {
  std::string path;

public:
  File();
  File(const std::string &);

  const std::string &getPath() const;
};

#endif // FILE_H_
