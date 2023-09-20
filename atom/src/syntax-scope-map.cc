#include "syntax-scope-map.h"

namespace {

enum struct NodeType {
  tag,
  string,
  universal,
  combinator,
  pseudo,
};

struct Node {
  NodeType type;
  std::string value;
  double indexValue;
};

static bool isNameChar(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-';
}

template <typename F> static void process(F f, const std::string &selector) {
  std::vector<Node> nodes;
  for (size_t i = 0; i < selector.size();) {
    const char c = selector[i];
    if (isNameChar(selector[i])) {
      const size_t start = i;
      while (i < selector.size() && isNameChar(selector[i])) {
        i++;
      }
      nodes.push_back({NodeType::tag, selector.substr(start, i - start)});
    } else if (selector[i] == '"') {
      const size_t start = i;
      i++;
      while (i < selector.size() && selector[i] != '"') {
        i++;
      }
      if (i < selector.size() && selector[i] == '"') {
        i++;
      }
      nodes.push_back({NodeType::string, selector.substr(start, i - start)});
    } else if (selector[i] == '*') {
      const size_t start = i;
      i++;
      nodes.push_back({NodeType::universal, selector.substr(start, i - start)});
    } else if (selector[i] == '>') {
      const size_t start = i;
      i++;
      nodes.push_back({NodeType::combinator, selector.substr(start, i - start)});
    } else if (selector[i] == ':') {
      const size_t start = i;
      i++;
      while (i < selector.size() && isNameChar(selector[i])) {
        i++;
      }
      const size_t end = i;
      double index = 0;
      if (i < selector.size() && selector[i] == '(') {
        i++;
        while (i < selector.size() && selector[i] >= '0' && selector[i] <= '9') {
          index = index * 10 + (selector[i] - '0');
          i++;
        }
        if (i < selector.size() && selector[i] == ')') {
          i++;
        }
      }
      nodes.push_back({NodeType::pseudo, selector.substr(start, end - start), index});
    } else {
      i++;
    }
  }
  f(nodes);
}

}

static void setTableDefaults(std::unordered_map<std::string, std::unique_ptr<SyntaxScopeMap::Table>> &, bool);
static void mergeTable(SyntaxScopeMap::Table *, SyntaxScopeMap::Table *, bool = true);
static void rejectSelector(const std::string &);

SyntaxScopeMap::Result::~Result() {}

SyntaxScopeMap::Table::Table() {
  this->result = nullptr;
}

SyntaxScopeMap::Table::~Table() {}

SyntaxScopeMap::SyntaxScopeMap() {}

void SyntaxScopeMap::finalize() {
  setTableDefaults(this->namedScopeTable, true);
  setTableDefaults(this->anonymousScopeTable, false);
}

SyntaxScopeMap::~SyntaxScopeMap() {}

void SyntaxScopeMap::addSelector(const std::string &selector, std::shared_ptr<Result> result) {
  process([&](const std::vector<Node> &nodes) {
    std::unordered_map<std::string, std::unique_ptr<Table>> *currentMap = nullptr;
    Table *currentTable = nullptr;
    optional<double> currentIndexValue;

    for (double i = nodes.size() - 1.0; i >= 0; i--) {
      const Node &termNode = nodes[i];

      switch (termNode.type) {
        case NodeType::tag:
          if (!currentMap) currentMap = &this->namedScopeTable;
          if (!(*currentMap)[termNode.value])
            (*currentMap)[termNode.value] = std::unique_ptr<Table>(new Table());
          currentTable = (*currentMap)[termNode.value].get();
          if (currentIndexValue) {
            if (!currentTable->indices[*currentIndexValue])
              currentTable->indices[*currentIndexValue] = std::unique_ptr<Table>(new Table());
            currentTable = currentTable->indices[*currentIndexValue].get();
            currentIndexValue = optional<double>();
          }
          break;

        case NodeType::string:
          {
            if (!currentMap) currentMap = &this->anonymousScopeTable;
            const std::string value = termNode.value.substr(1, termNode.value.size() - 2); //.replace(/\\"/g, '"');
            if (!(*currentMap)[value]) (*currentMap)[value] = std::unique_ptr<Table>(new Table());
            currentTable = (*currentMap)[value].get();
          }
          if (currentIndexValue) {
            if (!currentTable->indices[*currentIndexValue])
              currentTable->indices[*currentIndexValue] = std::unique_ptr<Table>(new Table());
            currentTable = currentTable->indices[*currentIndexValue].get();
            currentIndexValue = optional<double>();
          }
          break;

        case NodeType::universal:
          if (currentMap) {
            if (!(*currentMap)["*"]) (*currentMap)["*"] = std::unique_ptr<Table>(new Table());
            currentTable = (*currentMap)["*"].get();
          } else {
            if (!this->namedScopeTable["*"]) {
              this->namedScopeTable["*"] = std::unique_ptr<Table>(new Table());
              //this->anonymousScopeTable["*"] = std::unique_ptr<Table>(new Table());
            }
            currentTable = this->namedScopeTable["*"].get();
          }
          if (currentIndexValue) {
            if (!currentTable->indices[*currentIndexValue])
              currentTable->indices[*currentIndexValue] = std::unique_ptr<Table>(new Table());
            currentTable = currentTable->indices[*currentIndexValue].get();
            currentIndexValue = optional<double>();
          }
          break;

        case NodeType::combinator:
          if (currentIndexValue) {
            rejectSelector(selector);
          }

          if (termNode.value == ">") {
            currentMap = &currentTable->parents;
          } else {
            rejectSelector(selector);
          }
          break;

        case NodeType::pseudo:
          if (termNode.value == ":nth-child") {
            currentIndexValue = termNode.indexValue;
          } else {
            rejectSelector(selector);
          }
          break;

        default:
          break;
      }
    }

    currentTable->result = result;
  }, selector);
}

SyntaxScopeMap::Result *SyntaxScopeMap::get(const std::vector<std::string> &nodeTypes, const std::vector<double> &childIndices, bool leafIsNamed) {
  Result *result = nullptr;
  size_t i = nodeTypes.size() - 1;
  Table *currentTable = leafIsNamed
    ? this->namedScopeTable[nodeTypes[i]].get()
    : this->anonymousScopeTable[nodeTypes[i]].get();

  if (!currentTable) currentTable = this->namedScopeTable["*"].get();

  while (currentTable) {
    if (currentTable->indices.count(childIndices[i])) {
      currentTable = currentTable->indices[childIndices[i]].get();
    }

    if (currentTable->result) {
      result = currentTable->result.get();
    }

    if (i == 0) break;
    i--;
    if (currentTable->parents.count(nodeTypes[i])) {
      currentTable = currentTable->parents[nodeTypes[i]].get();
    } else if (currentTable->parents.count("*")) {
      currentTable = currentTable->parents["*"].get();
    } else {
      currentTable = nullptr;
    }
  }

  return result;
}

static void setTableDefaults(std::unordered_map<std::string, std::unique_ptr<SyntaxScopeMap::Table>> &table, bool allowWildcardSelector) {
  SyntaxScopeMap::Table *defaultTypeTable = allowWildcardSelector && table.count("*") ? table["*"].get() : nullptr;

  for (auto &entry : table) {
    SyntaxScopeMap::Table *typeTable = entry.second.get();
    if (typeTable == defaultTypeTable) continue;

    if (defaultTypeTable) {
      mergeTable(typeTable, defaultTypeTable);
    }

    setTableDefaults(typeTable->parents, true);

    for (auto &key : typeTable->indices) {
      SyntaxScopeMap::Table *indexTable = key.second.get();
      mergeTable(indexTable, typeTable, false);
      setTableDefaults(indexTable->parents, true);
    }
  }
}

static void mergeTable(SyntaxScopeMap::Table *table, SyntaxScopeMap::Table *defaultTable, bool mergeIndices) {
  if (mergeIndices) {
    for (auto &entry : defaultTable->indices) {
      double key = entry.first;
      if (!table->indices[key]) table->indices[key] = std::unique_ptr<SyntaxScopeMap::Table>(new SyntaxScopeMap::Table());
      mergeTable(table->indices[key].get(), defaultTable->indices[key].get());
    }
  }

  for (auto &entry : defaultTable->parents) {
    const std::string &key = entry.first;
    if (!table->parents[key]) table->parents[key] = std::unique_ptr<SyntaxScopeMap::Table>(new SyntaxScopeMap::Table());
    mergeTable(table->parents[key].get(), defaultTable->parents[key].get());
  }

  if (defaultTable->result != nullptr && table->result == nullptr) {
    table->result = defaultTable->result;
  }
}

static void rejectSelector(const std::string &selector) {
  //throw new TypeError(`Unsupported selector '${selector}'`);
}
