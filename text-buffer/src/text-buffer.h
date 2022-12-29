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
  double transactCallDepth;

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

  class SearchCallbackArgument {
    TextBuffer *buffer;
  public:
    Range range;
    bool stopped;
    optional<std::u16string> replacementText;
    SearchCallbackArgument(TextBuffer *, Range);
    std::u16string getMatchText();
    Range replace(std::u16string);
    void stop();
  };
  using ScanIterator = std::function<void(SearchCallbackArgument &)>;

  bool isModified();
  bool isEmpty() const;
  std::u16string getText();
  uint16_t getCharacterAtPosition(Point);
  std::u16string getTextInRange(Range);
  std::vector<std::u16string> getLines();
  optional<std::u16string> getLastLine();
  optional<std::u16string> lineForRow(double);
  const char16_t *lineEndingForRow(double);
  double lineLengthForRow(double);
  bool isRowBlank(double);
  optional<double> previousNonBlankRow(double);
  optional<double> nextNonBlankRow(double);
  Range setText(std::u16string);
  Range setTextInRange(Range, std::u16string);
  Range insert(Point position, std::u16string text);
  Range append(std::u16string text);
  Range applyChange(Change, bool);
  Range delete_(Range range);
  Range deleteRow(double row);
  Range deleteRows(double startRow, double endRow);
  void emitDidChangeEvent(Range, Range);
  MarkerLayer *addMarkerLayer();
  MarkerLayer *getMarkerLayer(unsigned);
  MarkerLayer *getDefaultMarkerLayer();
  Marker *markRange(Range);
  Marker *markPosition(Point);
  std::vector<Marker *> getMarkers();
  Marker *getMarker(unsigned);
  std::size_t getMarkerCount();
  void scan(const Regex &, ScanIterator);
  void backwardsScan(const Regex &, ScanIterator);
  void scanInRange(const Regex &, Range, ScanIterator, bool = false);
  void backwardsScanInRange(const Regex &, Range, ScanIterator);
  double replace(const Regex &, std::u16string);
  optional<NativeRange> findSync(const Regex &);
  optional<NativeRange> findInRangeSync(const Regex &, Range);
  std::vector<NativeRange> findAllSync(const Regex &);
  std::vector<NativeRange> findAllInRangeSync(const Regex &, Range);
  Range getRange() const;
  double getLineCount() const;
  double getLastRow() const;
  Point getFirstPosition() const;
  Point getEndPosition() const;
  double getLength() const;
  double getMaxCharacterIndex();
  Range rangeForRow(double, bool = false);
  double characterIndexForPosition(Point);
  Point positionForCharacterIndex(double);
  Range clipRange(Range);
  Point clipPosition(Point);
  DisplayLayer *addDisplayLayer();
  DisplayLayer *getDisplayLayer(unsigned);
  void markerCreated(MarkerLayer *, Marker *);
  void markersUpdated(MarkerLayer *);
  unsigned getNextMarkerId();
};

#endif  // TEXT_BUFFER_H_
