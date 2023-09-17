#include "tree-sitter.h"
#include <native-point.h>

static const unsigned BYTES_PER_CHARACTER = 2;
static TSTreeCursor scratch_cursor = {nullptr, nullptr, {0, 0}};

static inline bool operator<=(const TSPoint &left, const TSPoint &right) {
  if (left.row < right.row) return true;
  if (left.row > right.row) return false;
  return left.column <= right.column;
}

static NativePoint PointToJS(TSPoint point) {
  return {point.row, point.column / BYTES_PER_CHARACTER};
}

static TSPoint PointFromJS(const NativePoint &point) {
  return {point.row, point.column * BYTES_PER_CHARACTER};
}

unsigned startIndex(TSNode node) {
  return ts_node_start_byte(node) / BYTES_PER_CHARACTER;
}

unsigned endIndex(TSNode node) {
  return ts_node_end_byte(node) / BYTES_PER_CHARACTER;
}

unsigned startIndex(TSRange range) {
  return range.start_byte / BYTES_PER_CHARACTER;
}

unsigned endIndex(TSRange range) {
  return range.end_byte / BYTES_PER_CHARACTER;
}

NativePoint startPosition(TSNode node) {
  return PointToJS(ts_node_start_point(node));
}

NativePoint endPosition(TSNode node) {
  return PointToJS(ts_node_end_point(node));
}

NativePoint startPosition(TSRange range) {
  return PointToJS(range.start_point);
}

NativePoint endPosition(TSRange range) {
  return PointToJS(range.end_point);
}

std::vector<TSNode> children(TSNode node) {
  std::vector<TSNode> result;
  ts_tree_cursor_reset(&scratch_cursor, node);
  if (ts_tree_cursor_goto_first_child(&scratch_cursor)) {
    do {
      TSNode child = ts_tree_cursor_current_node(&scratch_cursor);
      result.push_back(child);
    } while (ts_tree_cursor_goto_next_sibling(&scratch_cursor));
  }
  return result;
}

struct SymbolSet {
  std::basic_string<TSSymbol> symbols;
  void add(TSSymbol symbol) { symbols += symbol; }
  bool contains(TSSymbol symbol) { return symbols.find(symbol) != symbols.npos; }
};

static bool symbol_set_from_js(SymbolSet *symbols, const std::vector<std::string> &types, const TSLanguage *language) {
  unsigned symbol_count = ts_language_symbol_count(language);

  for (size_t i = 0, n = types.size(); i < n; i++) {
    const std::string &node_type = types[i];

    if (node_type == "ERROR") {
      symbols->add(static_cast<TSSymbol>(-1));
    } else {
      for (TSSymbol j = 0; j < symbol_count; j++) {
        if (node_type == ts_language_symbol_name(language, j)) {
          symbols->add(j);
        }
      }
    }
  }

  return true;
}

std::vector<TSNode> descendantsOfType(TSNode node, const std::vector<std::string> &types, const NativePoint &start, const NativePoint &end) {
  SymbolSet symbols;
  symbol_set_from_js(&symbols, types, ts_tree_language(node.tree));

  TSPoint start_point = PointFromJS(start);
  TSPoint end_point = PointFromJS(end);

  std::vector<TSNode> found;
  ts_tree_cursor_reset(&scratch_cursor, node);
  auto already_visited_children = false;
  while (true) {
    TSNode descendant = ts_tree_cursor_current_node(&scratch_cursor);

    if (!already_visited_children) {
      if (ts_node_end_point(descendant) <= start_point) {
        if (ts_tree_cursor_goto_next_sibling(&scratch_cursor)) {
          already_visited_children = false;
        } else {
          if (!ts_tree_cursor_goto_parent(&scratch_cursor)) break;
          already_visited_children = true;
        }
        continue;
      }

      if (end_point <= ts_node_start_point(descendant)) break;

      if (symbols.contains(ts_node_symbol(descendant))) {
        found.push_back(descendant);
      }

      if (ts_tree_cursor_goto_first_child(&scratch_cursor)) {
        already_visited_children = false;
      } else if (ts_tree_cursor_goto_next_sibling(&scratch_cursor)) {
        already_visited_children = false;
      } else {
        if (!ts_tree_cursor_goto_parent(&scratch_cursor)) break;
        already_visited_children = true;
      }
    } else {
      if (ts_tree_cursor_goto_next_sibling(&scratch_cursor)) {
        already_visited_children = false;
      } else {
        if (!ts_tree_cursor_goto_parent(&scratch_cursor)) break;
      }
    }
  }

  return found;
}
