#ifndef DEFAULT_HISTORY_PROVIDER_H_
#define DEFAULT_HISTORY_PROVIDER_H_

#include <patch.h>
#include <vector>
#include <string>
#include "point.h"

class DefaultHistoryProvider {
  struct Checkpoint {
    unsigned id;
    bool isBarrier;
    Checkpoint(unsigned, bool);
  };
  struct Transaction {
    Patch patch;
    double groupingInterval;
    double timestamp;
    Transaction(Patch &&, double = 0);
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
    StackEntry &operator =(StackEntry &&);
  };
  size_t maxUndoEntries;
  unsigned nextCheckpointId;
  std::vector<StackEntry> undoStack;
  std::vector<StackEntry> redoStack;

public:
  DefaultHistoryProvider();
  ~DefaultHistoryProvider();

  unsigned createCheckpoint(bool);
  void groupChangesSinceCheckpoint(unsigned, bool);
  void enforceUndoStackSizeLimit();
  void applyGroupingInterval(double);
  void pushChange(Point, Point, Point, const std::u16string &, const std::u16string &);
  void pushPatch(Patch *patch);
  Patch undo();
  Patch redo();
  void clear();
  void clearUndoStack();
  void clearRedoStack();
};

#endif // DEFAULT_HISTORY_PROVIDER_H_
