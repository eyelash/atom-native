#ifndef BRACKET_MATCHER_VIEW_H_
#define BRACKET_MATCHER_VIEW_H_

#include <range.h>
#include <optional.h>
#include <tree_sitter/api.h>
#include <string>

struct TextEditor;
struct MatchManager;
struct TreeSitterLanguageMode;
struct DisplayMarker;

struct BracketMatcherView {
  TextEditor *editor;
  MatchManager *matchManager;
  bool pairHighlighted;
  bool tagHighlighted;
  DisplayMarker *startMarker;
  DisplayMarker *endMarker;

  BracketMatcherView(TextEditor *, MatchManager *);
  ~BracketMatcherView();

  void updateMatch();
  optional<Point> findMatchingEndBracket(const Point &, char16_t, char16_t);
  optional<Point> findMatchingStartBracket(const Point &, char16_t, char16_t);
  optional<Point> findMatchingEndBracketWithSyntaxTree(const Point &, char16_t, char16_t, TreeSitterLanguageMode *);
  optional<Point> findMatchingStartBracketWithSyntaxTree(const Point &, char16_t, char16_t, TreeSitterLanguageMode *);
  std::pair<optional<Range>, optional<Range>> findMatchingTagNameRangesWithSyntaxTree(TreeSitterLanguageMode *);
  std::pair<TSNode, TSNode> findContainingTagsWithSyntaxTree(Point, TreeSitterLanguageMode *);
  DisplayMarker *createMarker(const Range &);
  std::tuple<optional<Point>, optional<Point>> findCurrentPair();
  TreeSitterLanguageMode *hasSyntaxTree();
};

#endif // BRACKET_MATCHER_VIEW_H_
