#include "tree-cursor.h"
#include "tree.h"

TreeCursor::TreeCursor(TSNode node, Tree *tree) : tree_cursor{ts_tree_cursor_new(node)}, tree{tree} {}

TreeCursor::TreeCursor(const TreeCursor &tree_cursor) : tree_cursor{ts_tree_cursor_copy(&tree_cursor.tree_cursor)}, tree{tree_cursor.tree} {}

TreeCursor::~TreeCursor() {
  ts_tree_cursor_delete(&this->tree_cursor);
}

TreeCursor &TreeCursor::operator =(const TreeCursor &tree_cursor) {
  ts_tree_cursor_delete(&this->tree_cursor);
  this->tree_cursor = ts_tree_cursor_copy(&tree_cursor.tree_cursor);
  this->tree = tree_cursor.tree;
  return *this;
}

TSNode TreeCursor::currentNode() const {
  return ts_tree_cursor_current_node(&this->tree_cursor);
}

bool TreeCursor::gotoParent() {
  return ts_tree_cursor_goto_parent(&this->tree_cursor);
}

bool TreeCursor::gotoNextSibling() {
  return ts_tree_cursor_goto_next_sibling(&this->tree_cursor);
}

bool TreeCursor::gotoFirstChild() {
  return ts_tree_cursor_goto_first_child(&this->tree_cursor);
}

optional<double> TreeCursor::gotoFirstChildForIndex(double index) {
  const int64_t child_index = ts_tree_cursor_goto_first_child_for_byte(&this->tree_cursor, index * 2);
  if (child_index < 0) {
    return optional<double>();
  } else {
    return optional<double>(child_index);
  }
}

double TreeCursor::startIndex() const {
  return ts_node_start_byte(this->currentNode()) / 2;
}

double TreeCursor::endIndex() const {
  return ts_node_end_byte(this->currentNode()) / 2;
}

NativePoint TreeCursor::startPosition() const {
  const TSPoint point = ts_node_start_point(this->currentNode());
  return NativePoint(point.row, point.column / 2);
}

NativePoint TreeCursor::endPosition() const {
  const TSPoint point = ts_node_end_point(this->currentNode());
  return NativePoint(point.row, point.column / 2);
}

const char *TreeCursor::nodeType() const {
  return ts_node_type(this->currentNode());
}

bool TreeCursor::nodeIsNamed() const {
  return ts_node_is_named(this->currentNode());
}

std::u16string TreeCursor::nodeText() const {
  return this->tree->getText(*this);
}
