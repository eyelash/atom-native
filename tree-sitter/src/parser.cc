#include "parser.h"
#include <tree_sitter/api.h>
#include <native-text-buffer.h>
#include <vector>

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

Parser::Parser() {
  this->parser = ts_parser_new();
}

Parser::~Parser() {
  ts_parser_delete(this->parser);
}

void Parser::setLanguage(const TSLanguage *language) {
  ts_parser_set_language(parser, language);
}

Tree Parser::parseTextBufferSync(NativeTextBuffer *text_buffer, const Tree &old_tree) {
  NativeTextBuffer::Snapshot *snapshot = text_buffer->create_snapshot();
  auto slices = snapshot->primitive_chunks();
  TextBufferInput input(&slices);
  TSTree *result = ts_parser_parse(parser, old_tree.tree, input.input());
  delete snapshot;
  return Tree(result, text_buffer);
}
