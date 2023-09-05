#include "text-buffer.h"
#include "default-history-provider.h"
#include "display-layer.h"
#include "helpers.h"
#include "point-helpers.h"
#include "language-mode.h"

TextBuffer::TextBuffer() {
  this->buffer = new NativeTextBuffer();
  this->historyProvider = new DefaultHistoryProvider();
  this->languageMode = new LanguageMode();
  this->nextMarkerLayerId = 0;
  this->nextDisplayLayerId = 0;
  this->defaultMarkerLayer = new MarkerLayer(this, this->nextMarkerLayerId++);
  this->markerLayers[this->defaultMarkerLayer->id] = this->defaultMarkerLayer;
  this->nextMarkerId = 1;
  this->transactCallDepth = 0;
  this->previousModifiedStatus = false;
}

TextBuffer::TextBuffer(const std::u16string &text) {
  this->buffer = new NativeTextBuffer(text);
  this->historyProvider = new DefaultHistoryProvider();
  this->languageMode = new LanguageMode();
  this->nextMarkerLayerId = 0;
  this->nextDisplayLayerId = 0;
  this->defaultMarkerLayer = new MarkerLayer(this, this->nextMarkerLayerId++);
  this->markerLayers[this->defaultMarkerLayer->id] = this->defaultMarkerLayer;
  this->nextMarkerId = 1;
  this->transactCallDepth = 0;
  this->previousModifiedStatus = false;
}

TextBuffer *TextBuffer::loadSync(const std::string &filePath) {
  TextBuffer *buffer = new TextBuffer();
  buffer->setPath(filePath);
  buffer->loadSync();
  return buffer;
}

TextBuffer::~TextBuffer() {
  for (auto &displayLayer : this->displayLayers) {
    delete displayLayer.second;
  }
  for (auto &markerLayer : this->markerLayers) {
    delete markerLayer.second;
  }
  delete this->buffer;
  delete this->historyProvider;
  delete this->languageMode;
}

/*
Section: Event Subscription
*/

void TextBuffer::onDidChange(std::function<void()> callback) {
  return this->didChangeTextEmitter.on(callback);
}

void TextBuffer::onDidChangeText(std::function<void()> callback) {
  return this->onDidChange(callback);
}

void TextBuffer::onDidChangeModified(std::function<void()> callback) {
  return this->didChangeModifiedEmitter.on(callback);
}

void TextBuffer::onDidChangePath(std::function<void()> callback) {
  return this->didChangePathEmitter.on(callback);
}

void TextBuffer::onWillSave(std::function<void()> callback) {
  return this->willSaveEmitter.on(callback);
}

/*
Section: File Details
*/

bool TextBuffer::isModified() {
  if (this->file) {
    return /* !this.file.existsSync() || */ this->buffer->is_modified();
  } else {
    return this->buffer->size() > 0;
  }
}

optional<std::string> TextBuffer::getPath() {
  return this->file ? this->file->getPath() : optional<std::string>();
  //return this->file->getPath();
}

void TextBuffer::setPath(const std::string &filePath) {
  //if (filePath == this->getPath()) return;
  return this->setFile(File(filePath));
}

void TextBuffer::setFile(const File &file) {
  //if (file.getPath() == this->getPath()) return;

  this->file = file;
  //this->subscribeToFile();

  this->didChangePathEmitter.emit();
}

optional<std::string> TextBuffer::getEncoding() {
  return std::string("UTF-8");
}

optional<std::string> TextBuffer::getUri() {
  return this->getPath();
}

/*
Section: Reading Text
*/

bool TextBuffer::isEmpty() const {
  return this->buffer->size() == 0;
}

std::u16string TextBuffer::getText() {
  return buffer->text();
}

uint16_t TextBuffer::getCharacterAtPosition(Point position) {
  return this->buffer->character_at(position);
}

std::u16string TextBuffer::getTextInRange(Range range) {
  return this->buffer->text_in_range(range);
}

std::vector<std::u16string> TextBuffer::getLines() {
  std::vector<std::u16string> result;

  for (uint32_t row = 0, row_count = buffer->extent().row + 1; row < row_count; row++) {
    auto text = buffer->text_in_range({{row, 0}, {row, UINT32_MAX}});
    result.push_back(std::move(text));
  }

  return result;
}

std::u16string TextBuffer::getLastLine() {
  return this->lineForRow(this->getLastRow());
}

std::u16string TextBuffer::lineForRow(double row) {
  return *this->buffer->line_for_row(row);
}

const char16_t *TextBuffer::lineEndingForRow(double row) {
  return this->buffer->line_ending_for_row(row);
}

double TextBuffer::lineLengthForRow(double row) {
  return *this->buffer->line_length_for_row(row);
}

bool TextBuffer::isRowBlank(double row) {
  return !Regex(u"\\S").match(this->lineForRow(row));
}

optional<double> TextBuffer::previousNonBlankRow(double startRow) {
  if (startRow == 0) return optional<double>();
  startRow = std::min(startRow, this->getLastRow());
  for (double row = startRow - 1; row >= 0; row--) {
    if (!this->isRowBlank(row)) return row;
  }
  return optional<double>();
}

optional<double> TextBuffer::nextNonBlankRow(double startRow) {
  const double lastRow = this->getLastRow();
  if (startRow < lastRow) {
    for (double row = startRow + 1; row <= lastRow; row++) {
      if (!this->isRowBlank(row)) return row;
    }
  }
  return optional<double>();
}

/*
Section: Mutating Text
*/

Range TextBuffer::setText(std::u16string text) {
  return this->setTextInRange(this->getRange(), std::move(text));
}

Range TextBuffer::setTextInRange(Range range, std::u16string newText) {
  if (this->transactCallDepth == 0) {
    //const Range newRange = this->transact([&]() { this->setTextInRange(range, newText /* , {normalizeLineEndings} */); });
    //if (undo === 'skip') this.groupLastChanges()
    //return newRange;
  }

  const Range oldRange = this->clipRange(range);
  std::u16string oldText = this->getTextInRange(oldRange);

  const Range newRange = this->applyChange(
    oldRange.start,
    oldRange.end,
    oldRange.start,
    traverse(oldRange.start, extentForText(newText)),
    oldText,
    newText,
    true
  );
  return newRange;
}

Range TextBuffer::insert(Point position, std::u16string text) {
  return this->setTextInRange(Range(position, position), std::move(text));
}

Range TextBuffer::append(std::u16string text) {
  return this->insert(this->getEndPosition(), std::move(text));
}

Range TextBuffer::applyChange(Point oldStart, Point oldEnd, Point newStart, Point newEnd, const std::u16string &oldText, const std::u16string &newText, bool pushToHistory) {

  const Point oldExtent = traversal(oldEnd, oldStart);
  const Range oldRange = Range(newStart, traverse(newStart, oldExtent));

  const Point newExtent = traversal(newEnd, newStart);
  const Range newRange = Range(newStart, traverse(newStart, newExtent));

  if (pushToHistory) {
    this->historyProvider->pushChange(newStart, oldExtent, newExtent, oldText, newText);
  }

  //const changeEvent = {oldRange, newRange, oldText, newText}
  for (auto &displayLayer : this->displayLayers) {
    displayLayer.second->bufferWillChange(oldRange);
  }

  //this.emitWillChangeEvent()
  this->buffer->set_text_in_range(oldRange, std::u16string(newText));

  for (auto &markerLayer : this->markerLayers) {
    markerLayer.second->splice(oldRange.start, oldExtent, newExtent);
    this->markerLayersWithPendingUpdateEvents.insert(markerLayer.second);
  }

  //this.cachedText = null
  //this.changesSinceLastDidChangeTextEvent.push(change)
  //this.changesSinceLastStoppedChangingEvent.push(change)
  this->emitDidChangeEvent(oldRange, newRange, oldText, newText);
  return newRange;
}

void TextBuffer::emitDidChangeEvent(Range oldRange, Range newRange, const std::u16string &oldText, const std::u16string &newText) {
  if (!oldRange.isEmpty() || !newRange.isEmpty()) {
    this->languageMode->bufferDidChange(oldRange, newRange, oldText, newText);
    for (auto &displayLayer : this->displayLayers) {
      displayLayer.second->bufferDidChange(oldRange, newRange);
    }
  }
}

Range TextBuffer::delete_(Range range) {
  return this->setTextInRange(range, u"");
}

Range TextBuffer::deleteRow(double row) {
  return this->deleteRows(row, row);
}

Range TextBuffer::deleteRows(double startRow, double endRow) {
  Point endPoint, startPoint;
  const double lastRow = this->getLastRow();

  if (startRow > endRow) { std::swap(startRow, endRow); }

  if (endRow < 0) {
    return Range(this->getFirstPosition(), this->getFirstPosition());
  }

  if (startRow > lastRow) {
    return Range(this->getEndPosition(), this->getEndPosition());
  }

  startRow = std::max(0.0, startRow);
  endRow = std::min(lastRow, endRow);

  if (endRow < lastRow) {
    startPoint = Point(startRow, 0);
    endPoint = Point(endRow + 1, 0);
  } else {
    if (startRow == 0) {
      startPoint = Point(startRow, 0);
    } else {
      startPoint = Point(startRow - 1, this->lineLengthForRow(startRow - 1));
    }
    endPoint = Point(endRow, this->lineLengthForRow(endRow));
  }

  return this->delete_(Range(startPoint, endPoint));
}

/*
Section: Markers
*/

MarkerLayer *TextBuffer::addMarkerLayer(bool maintainHistory) {
  MarkerLayer *layer = new MarkerLayer(this, this->nextMarkerLayerId++, maintainHistory);
  this->markerLayers[layer->id] = layer;
  return layer;
}

MarkerLayer *TextBuffer::getMarkerLayer(unsigned id) {
  return this->markerLayers[id];
}

MarkerLayer *TextBuffer::getDefaultMarkerLayer() {
  return this->defaultMarkerLayer;
}

unsigned TextBuffer::markRange(Range range) {
  return this->defaultMarkerLayer->markRange(range);
}

unsigned TextBuffer::markPosition(Point position) {
  return this->defaultMarkerLayer->markPosition(position);
}

std::vector<Marker *> TextBuffer::getMarkers() {
  return this->defaultMarkerLayer->getMarkers();
}

Marker *TextBuffer::getMarker(unsigned id) {
  return this->defaultMarkerLayer->getMarker(id);
}

std::size_t TextBuffer::getMarkerCount() {
  return this->defaultMarkerLayer->getMarkerCount();
}

/*
Section: History
*/

bool TextBuffer::undo(DisplayMarkerLayer *selectionsMarkerLayer) {
  auto pop = this->historyProvider->undo();
  if (!pop) return false;

  this->transactCallDepth++;
  for (Patch::Change &change : pop.textUpdates()) {
    this->applyChange(change.old_start, change.old_end, change.new_start, change.new_end, change.old_text->content, change.new_text->content);
  }
  this->transactCallDepth--;
  this->restoreFromMarkerSnapshot(pop.markers, selectionsMarkerLayer);
  this->emitDidChangeTextEvent();
  this->emitMarkerChangeEvents(pop.markers);
  return true;
}

bool TextBuffer::redo(DisplayMarkerLayer *selectionsMarkerLayer) {
  auto pop = this->historyProvider->redo();
  if (!pop) return false;

  this->transactCallDepth++;
  for (Patch::Change &change : pop.textUpdates()) {
    this->applyChange(change.old_start, change.old_end, change.new_start, change.new_end, change.old_text->content, change.new_text->content);
  }
  this->transactCallDepth--;
  this->restoreFromMarkerSnapshot(pop.markers, selectionsMarkerLayer);
  this->emitDidChangeTextEvent();
  this->emitMarkerChangeEvents(pop.markers);
  return true;
}

void TextBuffer::transact(double groupingInterval, DisplayMarkerLayer *selectionsMarkerLayer, std::function<void()> fn) {
  const auto checkpointBefore = this->historyProvider->createCheckpoint(
    this->createMarkerSnapshot(selectionsMarkerLayer),
    true
  );

  this->transactCallDepth++;
  fn();
  this->transactCallDepth--;

  auto endMarkerSnapshot = this->createMarkerSnapshot(selectionsMarkerLayer);
  this->historyProvider->groupChangesSinceCheckpoint(checkpointBefore,
    endMarkerSnapshot,
    true
  );
  this->historyProvider->applyGroupingInterval(groupingInterval);
  this->historyProvider->enforceUndoStackSizeLimit();
  this->emitDidChangeTextEvent();
  this->emitMarkerChangeEvents(endMarkerSnapshot);
}

void TextBuffer::transact(std::function<void()> fn) {
  this->transact(0, nullptr, fn);
}

/*
Section: Search And Replace
*/

void TextBuffer::scan(const Regex &regex, /* options = {}, */ ScanIterator iterator) {
  /*if (_.isFunction(options)) {
    iterator = options
    options = {}
  }*/

  return this->scanInRange(regex, this->getRange(), /* options, */ iterator);
}

void TextBuffer::backwardsScan(const Regex &regex, /* options = {}, */ ScanIterator iterator) {
  /*if (_.isFunction(options)) {
    iterator = options
    options = {}
  }*/

  return this->backwardsScanInRange(regex, this->getRange(), /* options, */ iterator);
}

void TextBuffer::scanInRange(const Regex &regex, Range range, /* options = {}, */ ScanIterator callback, bool reverse) {
  /*if (_.isFunction(options)) {
    reverse = callback
    callback = options
    options = {}
  }*/

  range = this->clipRange(range);
  const auto matchRanges = this->findAllInRangeSync(regex, range);
  double startIndex = 0;
  double endIndex = matchRanges.size();
  double increment = 1;
  double previousRow = -1;
  double replacementColumnDelta = 0;
  if (reverse) {
    startIndex = matchRanges.size() - 1;
    endIndex = -1;
    increment = -1;
  }

  for (double i = startIndex; i != endIndex; i += increment) {
    Range matchRange = matchRanges[i];
    if (range.end.isEqual(matchRange.start) && (range.end.column > 0)) continue;
    if (matchRange.start.row != previousRow) {
      replacementColumnDelta = 0;
    }
    previousRow = matchRange.start.row;
    matchRange.start.column += replacementColumnDelta;
    matchRange.end.column += replacementColumnDelta;

    SearchCallbackArgument argument = SearchCallbackArgument(this, matchRange, regex /* , options*/);
    callback(argument);
    if (argument.stopped /* || !regex.global */) break;

    if (!reverse && argument.replacementText) {
      replacementColumnDelta +=
        (matchRange.start.column + argument.replacementText->size()) -
        matchRange.end.column;
    }
  }
}

void TextBuffer::backwardsScanInRange(const Regex &regex, Range range, /* options = {}, */ ScanIterator iterator) {
  /*if (_.isFunction(options)) {
    iterator = options
    options = {}
  }*/

  return this->scanInRange(regex, range, /* options, */ iterator, true);
}

optional<NativeRange> TextBuffer::findSync(const Regex &regex) { return this->buffer->find(regex); }

optional<NativeRange> TextBuffer::findInRangeSync(const Regex &regex, Range range) { return this->buffer->find(regex, range); }

std::vector<NativeRange> TextBuffer::findAllSync(const Regex &regex) { return this->buffer->find_all(regex); }

std::vector<NativeRange> TextBuffer::findAllInRangeSync(const Regex &regex, Range range) { return this->buffer->find_all(regex, range); }

std::vector<Marker *> TextBuffer::findAndMarkAllInRangeSync(MarkerLayer *markerLayer, const Regex &regex, Range range /* , options = {} */) {
  const unsigned startId = this->nextMarkerId;
  const bool exclusive = true; // options.invalidate === 'inside' || !options.tailed;
  this->nextMarkerId += this->buffer->find_and_mark_all(
    *markerLayer->index,
    startId,
    exclusive,
    regex,
    range
  );
  std::vector<Marker *> markers;
  for (unsigned id = startId; id < this->nextMarkerId; id++) {
    Marker *marker = new Marker(id, markerLayer, {}, {}, true);
    markerLayer->markersById[id] = marker;
    markers.push_back(marker);
  }
  return markers;
}

/*
Section: Buffer Range Details
*/

Range TextBuffer::getRange() const {
  return Range(this->getFirstPosition(), this->getEndPosition());
}

double TextBuffer::getLineCount() const { return this->buffer->extent().row + 1; }

double TextBuffer::getLastRow() const {
  return this->getLineCount() - 1;
}

Point TextBuffer::getFirstPosition() const {
  return Point(0, 0);
}

Point TextBuffer::getEndPosition() const { return this->buffer->extent(); }

double TextBuffer::getLength() const { return this->buffer->size(); }

double TextBuffer::getMaxCharacterIndex() {
  return this->characterIndexForPosition(Point::INFINITY_);
}

Range TextBuffer::rangeForRow(double row, bool includeNewline) {
  row = std::min(row, this->getLastRow());
  if (includeNewline && row < this->getLastRow()) {
    return Range(Point(row, 0), Point(row + 1, 0));
  } else {
    return Range(Point(row, 0), Point(row, this->lineLengthForRow(row)));
  }
}

double TextBuffer::characterIndexForPosition(Point position) {
  return this->buffer->clip_position(position).offset;
}

Point TextBuffer::positionForCharacterIndex(double offset) {
  return this->buffer->position_for_offset(offset);
}

Range TextBuffer::clipRange(Range range) {
  const Point start = this->clipPosition(range.start);
  const Point end = this->clipPosition(range.end);
  if (range.start == start && range.end == end) {
    return range;
  } else {
    return Range(start, end);
  }
}

Point TextBuffer::clipPosition(Point position) {
  const double row = position.row, column = position.column;
  if (row < 0) {
    return this->getFirstPosition();
  } else if (row > this->getLastRow()) {
    return this->getEndPosition();
  } else if (column < 0) {
    return Point(row, 0);
  } else {
    const double lineLength = this->lineLengthForRow(row);
    /*if (column >= lineLength && row < this.getLastRow() && options && options.clipDirection === 'forward') {
      return new Point(row + 1, 0)
    } else */ if (column > lineLength) {
      return Point(row, lineLength);
    } else {
      return position;
    }
  }
}

/*
Section: Buffer Operations
*/

TextBuffer *TextBuffer::save() {
  return this->saveTo(*this->file);
}

TextBuffer *TextBuffer::saveAs(const std::string &filePath) {
  return this->saveTo(File(filePath));
}

TextBuffer *TextBuffer::saveTo(const File &file) {
  //if (!file) throw new Error("Can't save a buffer with no file")

  const std::string &filePath = file.getPath();

  //this.outstandingSaveCount++

  //mkdirp(path.dirname(filePath))
  this->willSaveEmitter.emit();
  this->buffer->save(filePath, *this->getEncoding());

  //this.outstandingSaveCount--

  this->setFile(file);
  //this.fileHasChangedSinceLastLoad = false;
  //this.digestWhenLastPersisted = this.buffer.baseTextDigest();
  //this.loaded = true;
  this->emitModifiedStatusChanged(false);
  //this.emitter.emit('did-save', {path: filePath});
  return this;
}

/*
Section: Display Layers
*/

DisplayLayer *TextBuffer::addDisplayLayer() {
  const unsigned id = this->nextDisplayLayerId++;
  DisplayLayer *displayLayer = new DisplayLayer(id, this);
  this->displayLayers[id] = displayLayer;
  return displayLayer;
}

DisplayLayer *TextBuffer::getDisplayLayer(unsigned id) {
  return this->displayLayers[id];
}

/*
Language Modes
*/

LanguageMode *TextBuffer::getLanguageMode() { return this->languageMode; }

void TextBuffer::setLanguageMode(LanguageMode *languageMode) {
  if (languageMode != this->languageMode) {
    LanguageMode *oldLanguageMode = this->languageMode;
    /*if (typeof this.languageMode.destroy === 'function') {
      this.languageMode.destroy()
    }*/
    this->languageMode = languageMode ? languageMode : new LanguageMode();
    for (auto &displayLayer : this->displayLayers) {
      displayLayer.second->bufferDidChangeLanguageMode();
    }
    this->didChangeLanguageModeEmitter.emit();
    delete oldLanguageMode;
  }
}

void TextBuffer::onDidChangeLanguageMode(std::function<void()> callback) {
  return this->didChangeLanguageModeEmitter.on(callback);
}

/*
Section: Private Utility Methods
*/

TextBuffer *TextBuffer::loadSync() {
  optional<Patch> patch = this->buffer->load(
    *this->getPath(),
    *this->getEncoding(),
    [&](double percentDone, const optional<Patch> &patch) {
      if (patch && patch->get_change_count() > 0) {
        /*checkpoint = this.historyProvider.createCheckpoint({
          markers: this.createMarkerSnapshot(),
          isBarrier: true
        })
        this.emitter.emit('will-reload')
        this.emitWillChangeEvent()*/
      }
    }
  );
  //this->finishLoading(std::move(patch));

  return this;
}

TextBuffer::MarkerSnapshot TextBuffer::createMarkerSnapshot(DisplayMarkerLayer *selectionsMarkerLayer) {
  MarkerSnapshot snapshot;
  for (auto &markerLayer : this->markerLayers) {
    const unsigned markerLayerId = markerLayer.first;
    if (!markerLayer.second->maintainHistory) continue;
    /*if (
      selectionsMarkerLayer &&
      markerLayer.getRole() === 'selections' &&
      markerLayerId !== selectionsMarkerLayer->id
    ) continue;*/
    snapshot[markerLayerId] = markerLayer.second->createSnapshot();
  }
  return snapshot;
}

void TextBuffer::restoreFromMarkerSnapshot(const MarkerSnapshot &snapshot, DisplayMarkerLayer *selectionsMarkerLayer) {
  //optional<unsigned> selectionsSnapshotId;
  if (selectionsMarkerLayer != nullptr) {
    // Do selective selections marker restoration only when snapshot includes single selections snapshot.
    /*const selectionsSnapshotIds = Object.keys(snapshot).filter(id => this.selectionsMarkerLayerIds.has(id))
    if (selectionsSnapshotIds.length === 1) {
      selectionsSnapshotId = selectionsSnapshotIds[0]
    }*/
  }

  for (const auto &layerSnapshot : snapshot) {
    const unsigned markerLayerId = layerSnapshot.first;
    /* if (markerLayerId == selectionsSnapshotId) {
      this.markerLayers[selectionsMarkerLayer.id].restoreFromSnapshot(
        layerSnapshot,
        markerLayerId !== selectionsMarkerLayer.id
      )
    } else */ if (this->markerLayers.count(markerLayerId)) {
      this->markerLayers[markerLayerId]->restoreFromSnapshot(layerSnapshot.second);
    }
  }
}

void TextBuffer::emitMarkerChangeEvents(MarkerSnapshot &snapshot) {
  if (this->transactCallDepth == 0) {
    while (this->markerLayersWithPendingUpdateEvents.size() > 0) {
      std::vector<MarkerLayer *> updatedMarkerLayers(this->markerLayersWithPendingUpdateEvents.begin(), this->markerLayersWithPendingUpdateEvents.end());
      this->markerLayersWithPendingUpdateEvents.clear();
      for (MarkerLayer *markerLayer : updatedMarkerLayers) {
        markerLayer->emitUpdateEvent();
        if (markerLayer == this->defaultMarkerLayer) {
          //this.emitter.emit('did-update-markers')
        }
      }
    }
  }

  for (auto &markerLayer : this->markerLayers) {
    const unsigned markerLayerId = markerLayer.first;
    markerLayer.second->emitChangeEvents(snapshot[markerLayerId]);
  }
}

void TextBuffer::emitDidChangeTextEvent() {
  //this->cachedHasAstral = null
  if (this->transactCallDepth == 0) {
    //if (this->changesSinceLastDidChangeTextEvent.size() > 0) {
      //const compactedChanges = patchFromChanges(this->changesSinceLastDidChangeTextEvent).getChanges()
      //this->changesSinceLastDidChangeTextEvent.clear();
      //if (compactedChanges.length > 0) {
        //const changeEvent = new ChangeEvent(this, compactedChanges);
        this->languageMode->bufferDidFinishTransaction(/* changeEvent */);
        this->didChangeTextEmitter.emit();
      //}
      // TODO: debounce
      this->emitDidStopChangingEvent();
      //this->_emittedWillChangeEvent = false;
    //}
    for (auto &displayLayer : this->displayLayers) {
      displayLayer.second->emitDeferredChangeEvents();
    }
  }
}

void TextBuffer::emitDidStopChangingEvent() {
  //if (this->destroyed) return;
  const bool modifiedStatus = this->isModified();
  /*const compactedChanges = Object.freeze(normalizePatchChanges(
    patchFromChanges(this->changesSinceLastStoppedChangingEvent).getChanges()
  ));*/
  //this->changesSinceLastStoppedChangingEvent.length = 0;
  //this->emitter.emit('did-stop-changing', {changes: compactedChanges});
  this->emitModifiedStatusChanged(modifiedStatus);
}

void TextBuffer::emitModifiedStatusChanged(bool modifiedStatus) {
  if (modifiedStatus == this->previousModifiedStatus) return;
  this->previousModifiedStatus = modifiedStatus;
  return this->didChangeModifiedEmitter.emit();
}

/*
Section: Private History Delegate Methods
*/

/*
Section: Private MarkerLayer Delegate Methods
*/

void TextBuffer::markerCreated(MarkerLayer *layer, Marker *marker) {
  if (layer == this->defaultMarkerLayer) {
    //return this.emitter.emit('did-create-marker', marker)
  }
}

void TextBuffer::markersUpdated(MarkerLayer *layer) {
  if (this->transactCallDepth == 0) {
    layer->emitUpdateEvent();
    if (layer == this->defaultMarkerLayer) {
      //return this.emitter.emit('did-update-markers')
    }
  } else {
    return this->markerLayersWithPendingUpdateEvents.insert(layer);
  }
}

unsigned TextBuffer::getNextMarkerId() { return this->nextMarkerId++; }

TextBuffer::SearchCallbackArgument::SearchCallbackArgument(TextBuffer *buffer, Range range, const Regex &regex) :
  buffer{buffer},
  range{range},
  regex{regex},
  stopped{false} {}

std::u16string TextBuffer::SearchCallbackArgument::getMatchText() {
  return this->buffer->getTextInRange(this->range);
}

Range TextBuffer::SearchCallbackArgument::replace(std::u16string text) {
  this->replacementText = text;
  return this->buffer->setTextInRange(this->range, std::move(text));
}

void TextBuffer::SearchCallbackArgument::stop() {
  this->stopped = true;
}
