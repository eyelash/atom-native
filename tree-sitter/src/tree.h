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
  Tree &operator =(const Tree &);
  operator bool() const;

  TreeCursor walk();
  void edit(const TSInputEdit &);

  friend class Parser;
};

#endif // TREE_H_
