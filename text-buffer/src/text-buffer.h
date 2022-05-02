#ifndef TEXT_BUFFER_H_
#define TEXT_BUFFER_H_

#include "range.h"
#include <string>
#include <unordered_map>
#include <native-text-buffer.h>

class LanguageMode;
class Marker;
class MarkerLayer;
class DisplayLayer;

class TextBuffer {
  NativeTextBuffer *buffer;
  unsigned nextMarkerLayerId;
  unsigned nextDisplayLayerId;
  MarkerLayer *defaultMarkerLayer;
  std::unordered_map<unsigned, DisplayLayer *> displayLayers;
  std::unordered_map<unsigned, MarkerLayer *> markerLayers;
  unsigned nextMarkerId;

public:
  LanguageMode *languageMode;

  TextBuffer();
  TextBuffer(const std::u16string &text);
  ~TextBuffer();

  struct Change {
    Point oldStart;
    Point oldEnd;
    Point newStart;
    Point newEnd;
    std::u16string oldText;
    std::u16string newText;
  };

  bool isEmpty() const;
  std::u16string getText();
  uint16_t getCharacterAtPosition(Point);
  std::u16string getTextInRange(Range);
  std::vector<std::u16string> getLines();
  optional<std::u16string> getLastLine();
  optional<std::u16string> lineForRow(uint32_t);
  const char16_t *lineEndingForRow(double);
  optional<uint32_t> lineLengthForRow(uint32_t);
  Range setText(std::u16string &&);
  Range setTextInRange(Range, std::u16string &&);
  Range applyChange(Change, bool);
  MarkerLayer *addMarkerLayer();
  MarkerLayer *getMarkerLayer(unsigned);
  MarkerLayer *getDefaultMarkerLayer();
  Marker *markRange(Range);
  Marker *markPosition(Point);
  std::vector<Marker *> getMarkers();
  Marker *getMarker(unsigned);
  std::size_t getMarkerCount();
  Range getRange() const;
  double getLineCount() const;
  unsigned getLastRow() const;
  Point getFirstPosition() const;
  Point getEndPosition() const;
  uint32_t getLength() const;
  uint32_t getMaxCharacterIndex();
  Range rangeForRow(unsigned, bool);
  uint32_t characterIndexForPosition(Point);
  Point positionForCharacterIndex(uint32_t);
  Range clipRange(Range);
  Point clipPosition(Point);
  DisplayLayer *addDisplayLayer();
  DisplayLayer *getDisplayLayer(unsigned);
  unsigned getNextMarkerId();
};

#endif  // TEXT_BUFFER_H_
