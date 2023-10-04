#include "tree-sitter.h"
#include <native-point.h>
#include <native-text-buffer.h>

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

unsigned startIndex(TSTreeCursor *tree_cursor) {
  return startIndex(ts_tree_cursor_current_node(tree_cursor));
}

unsigned endIndex(TSTreeCursor *tree_cursor) {
  return endIndex(ts_tree_cursor_current_node(tree_cursor));
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

NativePoint startPosition(TSTreeCursor *tree_cursor) {
  return startPosition(ts_tree_cursor_current_node(tree_cursor));
}

NativePoint endPosition(TSTreeCursor *tree_cursor) {
  return endPosition(ts_tree_cursor_current_node(tree_cursor));
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

namespace {

using std::vector;
using std::pair;

class TextBufferInput {
public:
  TextBufferInput(const vector<pair<const char16_t *, uint32_t>> *slices)
    : slices_(slices),
      byte_offset(0),
      slice_index_(0),
      slice_offset_(0) {}

  TSInput input() {
    return TSInput{this, Read, TSInputEncodingUTF16};
  }

private:
  void seek(uint32_t byte_offset) {
    this->byte_offset = byte_offset;

    uint32_t total_length = 0;
    uint32_t goal_index = byte_offset / 2;
    for (unsigned i = 0, n = this->slices_->size(); i < n; i++) {
      uint32_t next_total_length = total_length + this->slices_->at(i).second;
      if (next_total_length > goal_index) {
        this->slice_index_ = i;
        this->slice_offset_ = goal_index - total_length;
        return;
      }
      total_length = next_total_length;
    }

    this->slice_index_ = this->slices_->size();
    this->slice_offset_ = 0;
  }

  static const char *Read(void *payload, uint32_t byte, TSPoint position, uint32_t *length) {
    auto self = static_cast<TextBufferInput *>(payload);

    if (byte != self->byte_offset) self->seek(byte);

    if (self->slice_index_ == self->slices_->size()) {
      *length = 0;
      return "";
    }

    auto &slice = self->slices_->at(self->slice_index_);
    const char16_t *result = slice.first + self->slice_offset_;
    *length = 2 * (slice.second - self->slice_offset_);
    self->byte_offset += *length;
    self->slice_index_++;
    self->slice_offset_ = 0;
    return reinterpret_cast<const char *>(result);
  }

  const vector<pair<const char16_t *, uint32_t>> *slices_;
  uint32_t byte_offset;
  uint32_t slice_index_;
  uint32_t slice_offset_;
};

}

TSTree *parseTextBufferSync(TSParser *parser, NativeTextBuffer *text_buffer, TSTree *old_tree, const std::vector<TSRange> &included_ranges) {
  ts_parser_set_included_ranges(parser, included_ranges.data(), included_ranges.size());
  NativeTextBuffer::Snapshot *snapshot = text_buffer->create_snapshot();
  auto slices = snapshot->primitive_chunks();
  TextBufferInput input(&slices);
  TSTree *result = ts_parser_parse(parser, old_tree, input.input());

  delete snapshot;
  return result;
}
