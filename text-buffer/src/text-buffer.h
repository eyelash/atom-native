#ifndef TEXT_BUFFER_H_
#define TEXT_BUFFER_H_

#include "marker-layer.h"
#include "range.h"
#include "file.h"
#include "event-kit.h"
#include <string>
#include <unordered_map>
#include <native-text-buffer.h>
#include <flat_set.h>

struct DefaultHistoryProvider;
struct LanguageMode;
struct DisplayLayer;
struct DisplayMarkerLayer;

struct TextBuffer {
  optional<File> file;
  Emitter<> didChangeTextEmitter;
  Emitter<> didChangeModifiedEmitter;
  Emitter<> didChangePathEmitter;
  Emitter<> willSaveEmitter;
  Emitter<> didChangeLanguageModeEmitter;
  NativeTextBuffer *buffer;
  DefaultHistoryProvider *historyProvider;
  LanguageMode *languageMode;
  unsigned nextMarkerLayerId;
  unsigned nextDisplayLayerId;
  MarkerLayer *defaultMarkerLayer;
  std::unordered_map<unsigned, DisplayLayer *> displayLayers;
  std::unordered_map<unsigned, MarkerLayer *> markerLayers;
  flat_set<MarkerLayer *> markerLayersWithPendingUpdateEvents;
  unsigned nextMarkerId;
  double transactCallDepth;
  bool previousModifiedStatus;

  TextBuffer();
  TextBuffer(const std::u16string &);
  static TextBuffer *loadSync(const std::string &);
  ~TextBuffer();

  struct SearchCallbackArgument {
    TextBuffer *buffer;
    Range range;
    const Regex &regex;
    bool stopped;
    optional<std::u16string> replacementText;
    SearchCallbackArgument(TextBuffer *, Range, const Regex &);
    std::u16string getMatchText();
    Range replace(std::u16string);
    void stop();
  };
  using ScanIterator = std::function<void(SearchCallbackArgument &)>;
  using MarkerSnapshot = std::unordered_map<unsigned, MarkerLayer::Snapshot>;

  void onDidChange(std::function<void()>);
  void onDidChangeText(std::function<void()>);
  void onDidChangeModified(std::function<void()>);
  void onDidChangePath(std::function<void()>);
  void onWillSave(std::function<void()>);
  bool isModified();
  optional<std::string> getPath();
  void setPath(const std::string &);
  void setFile(const File &);
  optional<std::string> getEncoding();
  optional<std::string> getUri();
  bool isEmpty() const;
  std::u16string getText();
  uint16_t getCharacterAtPosition(const Point &);
  std::u16string getTextInRange(const Range &);
  std::vector<std::u16string> getLines();
  std::u16string getLastLine();
  std::u16string lineForRow(double);
  const char16_t *lineEndingForRow(double);
  double lineLengthForRow(double);
  bool isRowBlank(double);
  optional<double> previousNonBlankRow(double);
  optional<double> nextNonBlankRow(double);
  Range setText(const std::u16string &);
  Range setTextInRange(const Range &, const std::u16string &);
  Range insert(const Point &, const std::u16string &);
  Range append(const std::u16string &);
  Range applyChange(const Point &, const Point &, const Point &, const Point &, const std::u16string &, const std::u16string &, bool = false);
  Range delete_(const Range &);
  Range deleteRow(double);
  Range deleteRows(double, double);
  void emitDidChangeEvent(const Range &, const Range &, const std::u16string &, const std::u16string &);
  MarkerLayer *addMarkerLayer(bool = false);
  MarkerLayer *getMarkerLayer(unsigned);
  MarkerLayer *getDefaultMarkerLayer();
  Marker *markRange(const Range &);
  Marker *markPosition(Point);
  std::vector<Marker *> getMarkers();
  Marker *getMarker(unsigned);
  size_t getMarkerCount();
  bool undo(DisplayMarkerLayer * = nullptr);
  bool redo(DisplayMarkerLayer * = nullptr);
  void transact(double, DisplayMarkerLayer *, std::function<void()>);
  void transact(std::function<void()>);
  void scan(const Regex &, ScanIterator);
  void backwardsScan(const Regex &, ScanIterator);
  void scanInRange(const Regex &, Range, ScanIterator, bool = false);
  void backwardsScanInRange(const Regex &, const Range &, ScanIterator);
  double replace(const Regex &, std::u16string);
  optional<NativeRange> findSync(const Regex &);
  optional<NativeRange> findInRangeSync(const Regex &, const Range &);
  std::vector<NativeRange> findAllSync(const Regex &);
  std::vector<NativeRange> findAllInRangeSync(const Regex &, const Range &);
  std::vector<Marker *> findAndMarkAllInRangeSync(MarkerLayer *, const Regex &, const Range &);
  Range getRange() const;
  double getLineCount() const;
  double getLastRow() const;
  Point getFirstPosition() const;
  Point getEndPosition() const;
  double getLength() const;
  double getMaxCharacterIndex();
  Range rangeForRow(double, bool = false);
  double characterIndexForPosition(const Point &);
  Point positionForCharacterIndex(double);
  Range clipRange(const Range &);
  Point clipPosition(const Point &);
  TextBuffer *save();
  TextBuffer *saveAs(const std::string &);
  TextBuffer *saveTo(const File &);
  DisplayLayer *addDisplayLayer();
  DisplayLayer *getDisplayLayer(unsigned);
  LanguageMode *getLanguageMode();
  void setLanguageMode(LanguageMode *);
  void onDidChangeLanguageMode(std::function<void()>);
  TextBuffer *loadSync();
  MarkerSnapshot createMarkerSnapshot(DisplayMarkerLayer *);
  void restoreFromMarkerSnapshot(const MarkerSnapshot &, DisplayMarkerLayer *);
  void emitMarkerChangeEvents(MarkerSnapshot &);
  void emitDidChangeTextEvent();
  void emitDidStopChangingEvent();
  void emitModifiedStatusChanged(bool);
  void markerCreated(MarkerLayer *, Marker *);
  void markersUpdated(MarkerLayer *);
  unsigned getNextMarkerId();
};

#endif // TEXT_BUFFER_H_
