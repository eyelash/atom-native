#ifndef TREE_H_
#define TREE_H_

#include "tree-cursor.h"

class NativeTextBuffer;

class Tree {
  TSTree *tree;
  NativeTextBuffer *input;

  Tree(TSTree *, NativeTextBuffer *);
public:
  Tree();
  Tree(const Tree &);
  ~Tree();
  Tree &operator =(const Tree &);
  operator bool() const;

  void edit(const TSInputEdit &);
  TSNode rootNode();
  std::vector<TSRange> getChangedRanges(const Tree &);
  TreeCursor walk();
  std::u16string getText(const TreeCursor &);

  friend class Parser;
};

#endif // TREE_H_
