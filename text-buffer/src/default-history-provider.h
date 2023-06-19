#ifndef DEFAULT_HISTORY_PROVIDER_H_
#define DEFAULT_HISTORY_PROVIDER_H_

#include "text-buffer.h"
#include <patch.h>
#include <vector>
#include <string>
#include "point.h"

struct DefaultHistoryProvider {
  struct Checkpoint {
    unsigned id;
    TextBuffer::MarkerSnapshot snapshot;
    bool isBarrier;
    Checkpoint(unsigned, const TextBuffer::MarkerSnapshot &, bool);
  };
  struct Transaction {
    TextBuffer::MarkerSnapshot markerSnapshotBefore;
    Patch patch;
    TextBuffer::MarkerSnapshot markerSnapshotAfter;
    double groupingInterval;
    double timestamp;
    Transaction(const TextBuffer::MarkerSnapshot &, Patch &&, const TextBuffer::MarkerSnapshot &, double = 0);
    bool shouldGroupWith(Transaction *);
    Transaction *groupWith(Transaction *);
  };
  struct StackEntry {
    enum class Type {
      Checkpoint,
      Transaction,
      Patch
    };
    void *value;
    Type type;
    StackEntry(Checkpoint *);
    StackEntry(Transaction *);
    StackEntry(Patch *);
    StackEntry(StackEntry &&);
    ~StackEntry();
    StackEntry &operator=(StackEntry &&);
  };
  size_t maxUndoEntries;
  unsigned nextCheckpointId;
  std::vector<StackEntry> undoStack;
  std::vector<StackEntry> redoStack;

  struct Result {
    Patch patch;
    TextBuffer::MarkerSnapshot markers;
    Result();
    Result(Patch &&, TextBuffer::MarkerSnapshot &&);
    operator bool() const;
    std::vector<Patch::Change> textUpdates() const;
  };

  DefaultHistoryProvider();
  ~DefaultHistoryProvider();

  unsigned createCheckpoint(const TextBuffer::MarkerSnapshot &, bool);
  void groupChangesSinceCheckpoint(unsigned, const TextBuffer::MarkerSnapshot &, bool);
  void enforceUndoStackSizeLimit();
  void applyGroupingInterval(double);
  void pushChange(Point, Point, Point, const std::u16string &, const std::u16string &);
  void pushPatch(Patch *patch);
  Result undo();
  Result redo();
  void clear();
  void clearUndoStack();
  void clearRedoStack();
};

#endif // DEFAULT_HISTORY_PROVIDER_H_
