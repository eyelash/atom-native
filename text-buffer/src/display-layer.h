#ifndef DISPLAY_LAYER_H_
#define DISPLAY_LAYER_H_

#include "range.h"
#include <unordered_map>
#include <patch.h>

class TextBuffer;
class MarkerLayer;
class DisplayMarkerLayer;

class DisplayLayer {
  TextBuffer *buffer;
  std::vector<int> cachedScreenLines;
  std::unordered_map<unsigned, DisplayMarkerLayer *> displayMarkerLayersById;
  double tabLength;
  double softWrapColumn;
  double softWrapHangingIndent;
  double (*ratioForCharacter)(char16_t);
  bool (*isWrapBoundary)(char16_t, char16_t);
  char16_t foldCharacter;
  MarkerLayer *foldsMarkerLayer;
  Patch *spatialIndex;
  std::vector<double> tabCounts;
  std::vector<double> screenLineLengths;
  Point rightmostScreenPosition;
  double indexedBufferRowCount;

public:
  unsigned id;

  enum class ClipDirection {
    backward,
    forward,
    closest
  };

  DisplayLayer(unsigned, TextBuffer *);
  ~DisplayLayer();

  void clearSpatialIndex();
  DisplayMarkerLayer *addMarkerLayer();
  DisplayMarkerLayer *getMarkerLayer(unsigned);
  Range bufferRangeForFold(unsigned);
  Point translateBufferPosition(Point);
  Point translateBufferPositionWithSpatialIndex(Point, ClipDirection);
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
  void updateSpatialIndex(double, double, double, double /*, deadline = NullDeadline */);
  void populateSpatialIndexIfNeeded(double, double);
  double findBoundaryPrecedingBufferRow(double);
  double findBoundaryFollowingBufferRow(double);
  std::pair<double, double> findBoundaryFollowingScreenRow(double);
  std::unordered_map<double, std::unordered_map<double, Point>> computeFoldsInBufferRowRange(double, double);
  static bool isSoftWrapHunk(const Patch::Change &);
};

#endif  // DISPLAY_LAYER_H_
