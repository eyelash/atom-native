#include "tree.h"
#include <tree_sitter/api.h>
#include <native-text-buffer.h>

static TSTree *copy(const TSTree *tree) {
  if (tree) {
    return ts_tree_copy(tree);
  } else {
    return nullptr;
  }
}

Tree::Tree(TSTree *tree, NativeTextBuffer *input) : tree{tree}, input{input} {}

Tree::Tree() : tree{nullptr}, input{nullptr} {}

Tree::Tree(const Tree &tree) : tree{copy(tree.tree)}, input{tree.input} {}

Tree::~Tree() {
  if (tree) {
    ts_tree_delete(this->tree);
  }
}

Tree &Tree::operator =(const Tree &tree) {
  ts_tree_delete(this->tree);
  this->tree = tree.tree ? ts_tree_copy(tree.tree) : nullptr;
  this->input = tree.input;
  return *this;
}

Tree::operator bool() const {
  return this->tree != nullptr;
}

void Tree::edit(const TSInputEdit &edit) {
  ts_tree_edit(tree, &edit);
}

TSNode Tree::rootNode() {
  return ts_tree_root_node(tree);
}

std::vector<TSRange> Tree::getChangedRanges(const Tree &newTree) {
  uint32_t length;
  TSRange *ranges = ts_tree_get_changed_ranges(tree, newTree.tree, &length);
  std::vector<TSRange> result(ranges, ranges + length);
  free(ranges);
  return result;
}

TreeCursor Tree::walk() {
  return TreeCursor(this->rootNode(), this);
}

std::u16string Tree::getText(const TreeCursor &cursor) {
  return this->input->text_in_range({cursor.startPosition(), cursor.endPosition()});
}
