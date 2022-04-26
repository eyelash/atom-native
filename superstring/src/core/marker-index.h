#ifndef MARKER_INDEX_H_
#define MARKER_INDEX_H_

#include <random>
#include <unordered_map>
#include "flat_set.h"
#include "native-point.h"
#include "range.h"

class MarkerIndex {
public:
  using MarkerId = unsigned;
  using MarkerIdSet = flat_set<MarkerId>;

  struct SpliceResult {
    flat_set<MarkerId> touch;
    flat_set<MarkerId> inside;
    flat_set<MarkerId> overlap;
    flat_set<MarkerId> surround;
  };

  struct Boundary {
    NativePoint position;
    flat_set<MarkerId> starting;
    flat_set<MarkerId> ending;
  };

  struct BoundaryQueryResult {
    std::vector<MarkerId> containing_start;
    std::vector<Boundary> boundaries;
  };

  MarkerIndex(unsigned seed = 0u);
  ~MarkerIndex();
  int generate_random_number();
  void insert(MarkerId id, NativePoint start, NativePoint end);
  void set_exclusive(MarkerId id, bool exclusive);
  void remove(MarkerId id);
  bool has(MarkerId id);
  SpliceResult splice(NativePoint start, NativePoint old_extent, NativePoint new_extent);
  NativePoint get_start(MarkerId id) const;
  NativePoint get_end(MarkerId id) const;
  Range get_range(MarkerId id) const;

  int compare(MarkerId id1, MarkerId id2) const;
  flat_set<MarkerId> find_intersecting(NativePoint start, NativePoint end);
  flat_set<MarkerId> find_containing(NativePoint start, NativePoint end);
  flat_set<MarkerId> find_contained_in(NativePoint start, NativePoint end);
  flat_set<MarkerId> find_starting_in(NativePoint start, NativePoint end);
  flat_set<MarkerId> find_starting_at(NativePoint position);
  flat_set<MarkerId> find_ending_in(NativePoint start, NativePoint end);
  flat_set<MarkerId> find_ending_at(NativePoint position);
  BoundaryQueryResult find_boundaries_after(NativePoint start, size_t max_count);

  std::unordered_map<MarkerId, Range> dump();

private:
  friend class Iterator;

  struct Node {
    Node *parent;
    Node *left;
    Node *right;
    NativePoint left_extent;
    flat_set<MarkerId> left_marker_ids;
    flat_set<MarkerId> right_marker_ids;
    flat_set<MarkerId> start_marker_ids;
    flat_set<MarkerId> end_marker_ids;
    int priority;

    Node(Node *parent, NativePoint left_extent);
    bool is_marker_endpoint();
  };

  class Iterator {
  public:
    Iterator(MarkerIndex *marker_index);
    void reset();
    Node* insert_marker_start(const MarkerId &id, const NativePoint &start_position, const NativePoint &end_position);
    Node* insert_marker_end(const MarkerId &id, const NativePoint &start_position, const NativePoint &end_position);
    Node* insert_splice_boundary(const NativePoint &position, bool is_insertion_end);
    void find_intersecting(const NativePoint &start, const NativePoint &end, flat_set<MarkerId> *result);
    void find_contained_in(const NativePoint &start, const NativePoint &end, flat_set<MarkerId> *result);
    void find_starting_in(const NativePoint &start, const NativePoint &end, flat_set<MarkerId> *result);
    void find_ending_in(const NativePoint &start, const NativePoint &end, flat_set<MarkerId> *result);
    void find_boundaries_after(NativePoint start, size_t max_count, BoundaryQueryResult *result);
    std::unordered_map<MarkerId, Range> dump();

  private:
    void ascend();
    void descend_left();
    void descend_right();
    void move_to_successor();
    void seek_to_first_node_greater_than_or_equal_to(const NativePoint &position);
    void mark_right(const MarkerId &id, const NativePoint &start_position, const NativePoint &end_position);
    void mark_left(const MarkerId &id, const NativePoint &start_position, const NativePoint &end_position);
    Node* insert_left_child(const NativePoint &position);
    Node* insert_right_child(const NativePoint &position);
    void check_intersection(const NativePoint &start, const NativePoint &end, flat_set<MarkerId> *results);
    void cache_node_position() const;

    MarkerIndex *marker_index;
    Node *current_node;
    NativePoint current_node_position;
    NativePoint left_ancestor_position;
    NativePoint right_ancestor_position;
    std::vector<NativePoint> left_ancestor_position_stack;
    std::vector<NativePoint> right_ancestor_position_stack;
  };

  NativePoint get_node_position(const Node *node) const;
  void delete_node(Node *node);
  void delete_subtree(Node *node);
  void bubble_node_up(Node *node);
  void bubble_node_down(Node *node);
  void rotate_node_left(Node *pivot);
  void rotate_node_right(Node *pivot);
  void get_starting_and_ending_markers_within_subtree(const Node *node, flat_set<MarkerId> *starting, flat_set<MarkerId> *ending);
  void populate_splice_invalidation_sets(SpliceResult *invalidated, const Node *start_node, const Node *end_node, const flat_set<MarkerId> &starting_inside_splice, const flat_set<MarkerId> &ending_inside_splice);

  std::default_random_engine random_engine;
  std::uniform_int_distribution<int> random_distribution;
  Node *root;
  std::unordered_map<MarkerId, Node*> start_nodes_by_id;
  std::unordered_map<MarkerId, Node*> end_nodes_by_id;
  Iterator iterator;
  flat_set<MarkerId> exclusive_marker_ids;
  mutable std::unordered_map<const Node*, NativePoint> node_position_cache;
};

#endif // MARKER_INDEX_H_
