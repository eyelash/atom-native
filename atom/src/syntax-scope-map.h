#ifndef SYNTAX_SCOPE_MAP_H_
#define SYNTAX_SCOPE_MAP_H_

#include <optional.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

struct TreeCursor;

struct SyntaxScopeMap {
  struct Result {
    virtual ~Result();
    virtual optional<std::string> applyLeafRules(const TreeCursor &) = 0;
  };
  struct Table {
    std::unordered_map<double, std::unique_ptr<Table>> indices;
    std::unordered_map<std::string, std::unique_ptr<Table>> parents;
    std::shared_ptr<Result> result;
    Table();
    ~Table();
  };
  std::unordered_map<std::string, std::unique_ptr<Table>> namedScopeTable;
  std::unordered_map<std::string, std::unique_ptr<Table>> anonymousScopeTable;

  SyntaxScopeMap();
  void finalize();
  ~SyntaxScopeMap();

  void addSelector(const std::string &, std::shared_ptr<Result>);
  std::shared_ptr<Result> get(const std::vector<std::string> &, const std::vector<double> &, bool = true);
};

#endif // SYNTAX_SCOPE_MAP_H_
