#ifndef SYNTAX_SCOPE_MAP_H_
#define SYNTAX_SCOPE_MAP_H_

#include <optional.h>
#include <vector>
#include <string>

class SyntaxScopeMap {
public:
  optional<std::string> get(const std::vector<std::string> &, const std::vector<double> &, bool = true);
};

#endif // SYNTAX_SCOPE_MAP_H_
