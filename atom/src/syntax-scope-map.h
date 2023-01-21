#ifndef SYNTAX_SCOPE_MAP_H_
#define SYNTAX_SCOPE_MAP_H_

#include <optional.h>
#include <vector>
#include <string>
#include <unordered_map>

class SyntaxScopeMap {
  class Table {
  public:
    std::unordered_map<double, Table *> indices;
    std::unordered_map<std::string, Table *> parents;
    optional<std::string> result;
    Table();
    ~Table();
  };
  std::unordered_map<std::string, Table *> namedScopeTable;
  std::unordered_map<std::string, Table *> anonymousScopeTable;

public:
  SyntaxScopeMap();
  ~SyntaxScopeMap();

  void addSelector(const std::string &, const std::string &);
  optional<std::string> get(const std::vector<std::string> &, const std::vector<double> &, bool = true);
};

#endif // SYNTAX_SCOPE_MAP_H_
