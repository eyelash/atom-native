#ifndef TREE_CURSOR_H_
#define TREE_CURSOR_H_

#include <tree_sitter/api.h>
#include <optional.h>
#include <native-point.h>

class Tree;

class TreeCursor {
  TSTreeCursor tree_cursor;
  Tree *tree;
public:
  TreeCursor(TSNode, Tree *);
  TreeCursor(const TreeCursor &);
  ~TreeCursor();
  TreeCursor &operator=(const TreeCursor &);
  TSNode currentNode() const;
  bool gotoParent();
  bool gotoNextSibling();
  bool gotoFirstChild();
  optional<double> gotoFirstChildForIndex(double);
  double startIndex() const;
  double endIndex() const;
  NativePoint startPosition() const;
  NativePoint endPosition() const;
  const char *nodeType() const;
  bool nodeIsNamed() const;
  std::u16string nodeText() const;
};

#endif // TREE_CURSOR_H_
