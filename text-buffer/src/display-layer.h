#ifndef DISPLAY_LAYER_H_
#define DISPLAY_LAYER_H_

#include "range.h"
#include <unordered_map>
#include <patch.h>

class TextBuffer;
class ScreenLineBuilder;
class MarkerLayer;
class DisplayMarkerLayer;

class DisplayLayer {
public:
  struct ScreenLine {
    unsigned id;
    std::u16string lineText;
    std::vector<int32_t> tags;
    double softWrapIndent;
  };
  struct Invisibles {
    const char16_t *space;
    const char16_t *tab;
  };
  enum class ClipDirection {
    backward,
    forward,
    closest
  };

  unsigned id;
private:
  TextBuffer *buffer;
  ScreenLineBuilder *screenLineBuilder;
  std::vector<ScreenLine> cachedScreenLines;
  std::unordered_map<int32_t, int32_t> builtInScopeIdsByFlags;
  std::unordered_map<int32_t, std::u16string> builtInClassNamesByScopeId;
  int32_t nextBuiltInScopeId;
  std::unordered_map<unsigned, DisplayMarkerLayer *> displayMarkerLayersById;
  Invisibles invisibles;
  double tabLength;
  double softWrapColumn;
  double softWrapHangingIndent;
  bool showIndentGuides;
  double (*ratioForCharacter)(char16_t);
  bool (*isWrapBoundary)(char16_t, char16_t);
  char16_t foldCharacter;
  std::unordered_map<std::u16string, const char16_t *> eolInvisibles;
  MarkerLayer *foldsMarkerLayer;
  Patch *spatialIndex;
  std::vector<double> tabCounts;
  std::vector<double> screenLineLengths;
  Point rightmostScreenPosition;
  double indexedBufferRowCount;

public:
  DisplayLayer(unsigned, TextBuffer *);
  ~DisplayLayer();

  void clearSpatialIndex();
  DisplayMarkerLayer *addMarkerLayer();
  DisplayMarkerLayer *getMarkerLayer(unsigned);
  Range bufferRangeForFold(unsigned);
  Point translateBufferPosition(Point);
  Point translateBufferPositionWithSpatialIndex(Point, ClipDirection = ClipDirection::closest);
  Range translateBufferRange(Range);
  Point translateScreenPosition(Point);
  Point translateScreenPositionWithSpatialIndex(Point, ClipDirection = ClipDirection::forward, bool = false);
  Range translateScreenRange(Range);
  Point clipScreenPosition(Point);
  Point constrainScreenPosition(Point, ClipDirection);
  Point expandHardTabs(Point, Point, double);
  Point collapseHardTabs(Point, double, ClipDirection);
  double getClipColumnDelta(Point, ClipDirection);
  double lineLengthForScreenRow(double);
  double getLastScreenRow();
  double getScreenLineCount();
  Point getRightmostScreenPosition();
  Point getApproximateRightmostScreenPosition();
  std::vector<double> bufferRowsForScreenRows(double, double);
  ScreenLine getScreenLine(double);
  std::vector<ScreenLine> getScreenLines(double, double);
  std::vector<ScreenLine> getScreenLines(double = 0);
  double leadingWhitespaceLengthForSurroundingLines(double);
  double leadingWhitespaceLengthForNonEmptyLine(const std::u16string &);
  double findTrailingWhitespaceStartColumn(double);
  int32_t registerBuiltInScope(int32_t, const std::u16string &);
  int32_t getBuiltInScopeId(int32_t);
  std::u16string classNameForScopeId(int32_t);
  int32_t scopeIdForTag(int32_t) const;
  std::u16string classNameForTag(int32_t);
  int32_t openTagForScopeId(int32_t);
  int32_t closeTagForScopeId(int32_t);
  bool isOpenTag(int32_t) const;
  bool isCloseTag(int32_t) const;
  void updateSpatialIndex(double, double, double, double /*, deadline = NullDeadline */);
  void populateSpatialIndexIfNeeded(double, double);
  double findBoundaryPrecedingBufferRow(double);
  double findBoundaryFollowingBufferRow(double);
  std::pair<double, double> findBoundaryFollowingScreenRow(double);
  std::unordered_map<double, std::unordered_map<double, Point>> computeFoldsInBufferRowRange(double, double);
  static bool isSoftWrapHunk(const Patch::Change &);

  friend class ScreenLineBuilder;
};

#endif  // DISPLAY_LAYER_H_
