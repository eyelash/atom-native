#ifndef PATCH_H_
#define PATCH_H_

#include "optional.h"
#include "native-point.h"
#include "serializer.h"
#include "text.h"
#include <memory>
#include <vector>
#include <ostream>

class Patch {
  struct Node;
  struct OldCoordinates;
  struct NewCoordinates;
  struct PositionStackEntry;

  Node *root;
  std::vector<Node *> node_stack;
  std::vector<PositionStackEntry> left_ancestor_stack;
  uint32_t change_count;
  bool merges_adjacent_changes;

public:
  struct Change {
    NativePoint old_start;
    NativePoint old_end;
    NativePoint new_start;
    NativePoint new_end;
    Text *old_text;
    Text *new_text;
    uint32_t preceding_old_text_size;
    uint32_t preceding_new_text_size;
    uint32_t old_text_size;
  };

  // Construction and destruction
  Patch(bool merges_adjacent_changes = true);
  Patch(Patch &&);
  Patch(Deserializer &input);
  Patch &operator=(Patch &&);
  ~Patch();
  static Patch compose(const std::vector<Patch *> &);
  void serialize(Serializer &serializer);

  Patch copy();
  Patch invert();

  // Mutations
  bool splice(NativePoint new_splice_start,
              NativePoint new_deletion_extent, NativePoint new_insertion_extent,
              optional<Text> &&deleted_text = optional<Text>{},
              optional<Text> &&inserted_text = optional<Text>{},
              uint32_t deleted_text_size = 0);
  void splice_old(NativePoint start, NativePoint deletion_extent, NativePoint insertion_extent);
  bool combine(const Patch &other, bool left_to_right = true);
  void clear();
  void rebalance();

  // Non-splaying reads
  std::vector<Change> get_changes() const;
  size_t get_change_count() const;
  std::vector<Change> get_changes_in_old_range(NativePoint start, NativePoint end) const;
  std::vector<Change> get_changes_in_new_range(NativePoint start, NativePoint end) const;
  optional<Change> get_change_starting_before_old_position(NativePoint position) const;
  optional<Change> get_change_starting_before_new_position(NativePoint position) const;
  optional<Change> get_change_ending_after_new_position(NativePoint position) const;
  optional<Change> get_bounds() const;
  NativePoint new_position_for_new_offset(uint32_t new_offset,
                                    std::function<uint32_t(NativePoint)> old_offset_for_old_position,
                                    std::function<NativePoint(uint32_t)> old_position_for_old_offset) const;

  // Splaying reads
  std::vector<Change> grab_changes_in_old_range(NativePoint start, NativePoint end);
  std::vector<Change> grab_changes_in_new_range(NativePoint start, NativePoint end);
  optional<Change> grab_change_starting_before_old_position(NativePoint position);
  optional<Change> grab_change_starting_before_new_position(NativePoint position);
  optional<Change> grab_change_ending_after_new_position(NativePoint position, bool exclusive = false);

  // Debugging
  std::string get_dot_graph() const;
  std::string get_json() const;

private:
  Patch(Node *root, uint32_t change_count, bool merges_adjacent_changes);

  template <typename CoordinateSpace>
  std::vector<Change> get_changes_in_range(NativePoint, NativePoint, bool inclusive) const;

  template <typename CoordinateSpace>
  optional<Change> get_change_starting_before_position(NativePoint target) const;

  template <typename CoordinateSpace>
  optional<Change> get_change_ending_after_position(NativePoint target) const;

  template <typename CoordinateSpace>
  std::vector<Change> grab_changes_in_range(NativePoint, NativePoint, bool inclusive = false);

  template <typename CoordinateSpace>
  optional<Change> grab_change_starting_before_position(NativePoint position);

  template <typename CoordinateSpace>
  Node *splay_node_starting_before(NativePoint target);

  template <typename CoordinateSpace>
  Node *splay_node_starting_after(NativePoint target, optional<NativePoint> exclusive_lower_bound);

  template <typename CoordinateSpace>
  Node *splay_node_ending_before(NativePoint target);

  template <typename CoordinateSpace>
  Node *splay_node_ending_after(NativePoint target, optional<NativePoint> exclusive_lower_bound);

  Change change_for_root_node();

  std::pair<optional<Text>, bool> compute_old_text(optional<Text> &&, NativePoint, NativePoint);
  uint32_t compute_old_text_size(uint32_t, NativePoint, NativePoint);

  void splay_node(Node *);
  void rotate_node_right(Node *, Node *, Node *);
  void rotate_node_left(Node *, Node *, Node *);
  void delete_root();
  void perform_rebalancing_rotations(uint32_t);
  Node *build_node(Node *, Node *, NativePoint, NativePoint, NativePoint, NativePoint,
                  optional<Text> &&, optional<Text> &&, uint32_t old_text_size);
  void delete_node(Node **);
  void remove_noop_change();
};

std::ostream &operator<<(std::ostream &, const Patch::Change &);

#endif // PATCH_H_
