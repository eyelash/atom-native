#include "syntax-scope-map.h"

namespace {

enum class NodeType {
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

template <class F> static void process(F f, const std::string &selector) {
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

static void rejectSelector(const std::string &);

SyntaxScopeMap::Result::~Result() {}

SyntaxScopeMap::Table::Table() {
  this->result = nullptr;
}

SyntaxScopeMap::Table::~Table() {
  for (auto &table : this->indices) {
    delete table.second;
  }
  for (auto &table : this->parents) {
    delete table.second;
  }
  if (this->result) {
    delete this->result;
  }
}

SyntaxScopeMap::SyntaxScopeMap() {}

SyntaxScopeMap::~SyntaxScopeMap() {
  for (auto &table : this->namedScopeTable) {
    delete table.second;
  }
  for (auto &table : this->anonymousScopeTable) {
    delete table.second;
  }
}

void SyntaxScopeMap::addSelector(const std::string &selector, Result *result) {
  process([&](const std::vector<Node> &nodes) {
    std::unordered_map<std::string, Table *> *currentMap = nullptr;
    Table *currentTable = nullptr;
    optional<double> currentIndexValue;

    for (double i = nodes.size() - 1.0; i >= 0; i--) {
      const Node &termNode = nodes[i];

      switch (termNode.type) {
        case NodeType::tag:
          if (!currentMap) currentMap = &this->namedScopeTable;
          if (!(*currentMap)[termNode.value])
            (*currentMap)[termNode.value] = new Table();
          currentTable = (*currentMap)[termNode.value];
          if (currentIndexValue) {
            if (!currentTable->indices[*currentIndexValue])
              currentTable->indices[*currentIndexValue] = new Table();
            currentTable = currentTable->indices[*currentIndexValue];
            currentIndexValue = optional<double>();
          }
          break;

        case NodeType::string:
          {
            if (!currentMap) currentMap = &this->anonymousScopeTable;
            const std::string value = termNode.value.substr(1, termNode.value.size() - 2); //.replace(/\\"/g, '"');
            if (!(*currentMap)[value]) (*currentMap)[value] = new Table();
            currentTable = (*currentMap)[value];
          }
          if (currentIndexValue) {
            if (!currentTable->indices[*currentIndexValue])
              currentTable->indices[*currentIndexValue] = new Table();
            currentTable = currentTable->indices[*currentIndexValue];
            currentIndexValue = optional<double>();
          }
          break;

        case NodeType::universal:
          if (currentMap) {
            if (!(*currentMap)["*"]) (*currentMap)["*"] = new Table();
            currentTable = (*currentMap)["*"];
          } else {
            if (!this->namedScopeTable["*"]) {
              this->namedScopeTable["*"] = new Table();
              //this->anonymousScopeTable["*"] = new Table();
            }
            currentTable = this->namedScopeTable["*"];
          }
          if (currentIndexValue) {
            if (!currentTable->indices[*currentIndexValue])
              currentTable->indices[*currentIndexValue] = new Table();
            currentTable = currentTable->indices[*currentIndexValue];
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

    if (currentTable->result) delete currentTable->result;
    currentTable->result = result;
  }, selector);
}

SyntaxScopeMap::Result *SyntaxScopeMap::get(const std::vector<std::string> &nodeTypes, const std::vector<double> &childIndices, bool leafIsNamed) {
  Result *result = nullptr;
  size_t i = nodeTypes.size() - 1;
  Table *currentTable = leafIsNamed
    ? this->namedScopeTable[nodeTypes[i]]
    : this->anonymousScopeTable[nodeTypes[i]];

  if (!currentTable) currentTable = this->namedScopeTable["*"];

  while (currentTable) {
    if (currentTable->indices.count(childIndices[i])) {
      currentTable = currentTable->indices[childIndices[i]];
    }

    if (currentTable->result) {
      result = currentTable->result;
    }

    if (i == 0) break;
    i--;
    if (currentTable->parents.count(nodeTypes[i])) {
      currentTable = currentTable->parents[nodeTypes[i]];
    } else if (currentTable->parents.count("*")) {
      currentTable = currentTable->parents["*"];
    } else {
      currentTable = nullptr;
    }
  }

  return result;
}

static void rejectSelector(const std::string &selector) {
  //throw new TypeError(`Unsupported selector '${selector}'`);
}
