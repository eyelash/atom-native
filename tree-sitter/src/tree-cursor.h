#ifndef TREE_CURSOR_H_
#define TREE_CURSOR_H_

#include <tree_sitter/api.h>
#include <optional.h>
#include <point.h>

class TreeCursor {
  TSTreeCursor tree_cursor;
public:
  TreeCursor(TSNode);
  TreeCursor(const TreeCursor &);
  ~TreeCursor();
  TreeCursor &operator =(const TreeCursor &);
  TSNode currentNode() const;
  bool gotoParent();
  bool gotoNextSibling();
  bool gotoFirstChild();
  optional<double> gotoFirstChildForIndex(double);
  double startIndex() const;
  double endIndex() const;
  Point startPosition() const;
  Point endPosition() const;
  const char *nodeType() const;
  bool nodeIsNamed() const;
};

#endif // TREE_CURSOR_H_
