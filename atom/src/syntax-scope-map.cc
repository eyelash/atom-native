#include "syntax-scope-map.h"

optional<std::string> SyntaxScopeMap::get(const std::vector<std::string> &nodeTypes, const std::vector<double> &childIndices, bool leafIsNamed) {
  optional<std::string> result;
  size_t i = nodeTypes.size() - 1;
  if (leafIsNamed && nodeTypes[i] == "comment") {
    result = optional<std::string>("comment.block");
  } else if (!leafIsNamed && nodeTypes[i] == "struct") {
    result = optional<std::string>("keyword.control");
  }
  return result;
}
