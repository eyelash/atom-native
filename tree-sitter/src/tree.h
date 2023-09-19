#ifndef TREE_H_
#define TREE_H_

#include "tree-cursor.h"

class Tree {
  TSTree *tree;

  Tree(TSTree *);
public:
  Tree();
  Tree(const Tree &);
  ~Tree();
  Tree &operator=(const Tree &);
  operator bool() const;

  void edit(const TSInputEdit &);
  TSNode rootNode() const;
  std::vector<TSRange> getChangedRanges(const Tree &);
  TreeCursor walk();

  friend class Parser;
};

#endif // TREE_H_
