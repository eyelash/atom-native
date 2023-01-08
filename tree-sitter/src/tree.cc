#include "tree.h"
#include <tree_sitter/api.h>

static TSTree *copy(const TSTree *tree) {
  if (tree) {
    return ts_tree_copy(tree);
  } else {
    return nullptr;
  }
}

Tree::Tree(TSTree *tree) : tree{tree} {}

Tree::Tree() : tree{nullptr} {}

Tree::Tree(const Tree &tree) : tree{copy(tree.tree)} {}

Tree::~Tree() {
  if (tree) {
    ts_tree_delete(this->tree);
  }
}

Tree &Tree::operator =(const Tree &tree) {
  ts_tree_delete(this->tree);
  this->tree = tree.tree ? ts_tree_copy(tree.tree) : nullptr;
  return *this;
}

Tree::operator bool() const {
  return this->tree != nullptr;
}

TreeCursor Tree::walk() {
  return TreeCursor(ts_tree_root_node(tree));
}

void Tree::edit(const TSInputEdit &edit) {
  ts_tree_edit(tree, &edit);
}
