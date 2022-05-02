#ifndef SUPERSTRING_TEXT_BUFFER_H_
#define SUPERSTRING_TEXT_BUFFER_H_

#include <string>
#include <vector>
#include "text.h"
#include "patch.h"
#include "native-point.h"
#include "native-range.h"
#include "regex.h"
#include "marker-index.h"

class NativeTextBuffer {
  struct Layer;
  Layer *base_layer;
  Layer *top_layer;
  void squash_layers(const std::vector<Layer *> &);
  void consolidate_layers();

public:
  static uint32_t MAX_CHUNK_SIZE_TO_COPY;

  NativeTextBuffer();
  NativeTextBuffer(std::u16string &&);
  NativeTextBuffer(const std::u16string &text);
  ~NativeTextBuffer();

  uint32_t size() const;
  NativePoint extent() const;
  optional<std::u16string> line_for_row(uint32_t row);
  void with_line_for_row(uint32_t row, const std::function<void(const char16_t *, uint32_t)> &);

  optional<uint32_t> line_length_for_row(uint32_t row);
  const char16_t *line_ending_for_row(uint32_t row);
  ClipResult clip_position(NativePoint);
  NativePoint position_for_offset(uint32_t offset);
  std::u16string text();
  uint16_t character_at(NativePoint position) const;
  std::u16string text_in_range(NativeRange range);
  void set_text(std::u16string &&);
  void set_text(const std::u16string &);
  void set_text_in_range(NativeRange old_range, std::u16string &&);
  void set_text_in_range(NativeRange old_range, const std::u16string &);
  bool is_modified() const;
  bool has_astral();
  std::vector<TextSlice> chunks() const;

  void reset(Text &&);
  void flush_changes();
  void serialize_changes(Serializer &);
  bool deserialize_changes(Deserializer &);
  const Text &base_text() const;

  optional<NativeRange> find(const Regex &, NativeRange range = NativeRange::all_inclusive()) const;
  std::vector<NativeRange> find_all(const Regex &, NativeRange range = NativeRange::all_inclusive()) const;
  unsigned find_and_mark_all(MarkerIndex &, MarkerIndex::MarkerId, bool exclusive,
                             const Regex &, NativeRange range = NativeRange::all_inclusive()) const;

  struct SubsequenceMatch {
    std::u16string word;
    std::vector<NativePoint> positions;
    std::vector<uint32_t> match_indices;
    int32_t score;
    bool operator==(const SubsequenceMatch &) const;
  };

  std::vector<SubsequenceMatch> find_words_with_subsequence_in_range(const std::u16string &, const std::u16string &, NativeRange) const;

  class Snapshot {
    friend class NativeTextBuffer;
    NativeTextBuffer &buffer;
    Layer &layer;
    Layer &base_layer;

    Snapshot(NativeTextBuffer &, Layer &, Layer &);

  public:
    ~Snapshot();
    void flush_preceding_changes();

    uint32_t size() const;
    NativePoint extent() const;
    uint32_t line_length_for_row(uint32_t) const;
    std::vector<TextSlice> chunks() const;
    std::vector<TextSlice> chunks_in_range(NativeRange) const;
    std::vector<std::pair<const char16_t *, uint32_t>> primitive_chunks() const;
    std::u16string text() const;
    std::u16string text_in_range(NativeRange) const;
    const Text &base_text() const;
    optional<NativeRange> find(const Regex &, NativeRange range = NativeRange::all_inclusive()) const;
    std::vector<NativeRange> find_all(const Regex &, NativeRange range = NativeRange::all_inclusive()) const;
    std::vector<SubsequenceMatch> find_words_with_subsequence_in_range(std::u16string query, const std::u16string &extra_word_characters, NativeRange range) const;
  };

  friend class Snapshot;
  Snapshot *create_snapshot();

  bool is_modified(const Snapshot *) const;
  Patch get_inverted_changes(const Snapshot *) const;

  size_t layer_count()  const;
  std::string get_dot_graph() const;
};

#endif  // SUPERSTRING_TEXT_BUFFER_H_
