#ifndef SYNTAX_SCOPE_MAP_H_
#define SYNTAX_SCOPE_MAP_H_

#include <optional.h>
#include <vector>
#include <string>
#include <unordered_map>

class TreeCursor;

class SyntaxScopeMap {
public:
  class Result {
  public:
    virtual ~Result();
    virtual optional<std::string> applyLeafRules(const TreeCursor &) = 0;
  };
  class Table {
  public:
    std::unordered_map<double, Table *> indices;
    std::unordered_map<std::string, Table *> parents;
    Result *result;
    Table();
    ~Table();
  };
  std::unordered_map<std::string, Table *> namedScopeTable;
  std::unordered_map<std::string, Table *> anonymousScopeTable;

  SyntaxScopeMap();
  ~SyntaxScopeMap();

  void addSelector(const std::string &, Result *);
  Result *get(const std::vector<std::string> &, const std::vector<double> &, bool = true);
};

#endif // SYNTAX_SCOPE_MAP_H_
