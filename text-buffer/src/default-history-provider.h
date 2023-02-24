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
    Transaction(Patch &&);
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
  unsigned nextCheckpointId;
  std::vector<StackEntry> undoStack;
  std::vector<StackEntry> redoStack;

public:
  DefaultHistoryProvider();
  ~DefaultHistoryProvider();

  unsigned createCheckpoint(bool);
  void groupChangesSinceCheckpoint(unsigned, bool);
  void pushChange(Point, Point, Point, const std::u16string &, const std::u16string &);
  void pushPatch(Patch *patch);
  Patch undo();
  Patch redo();
  void clear();
  void clearUndoStack();
  void clearRedoStack();
};

#endif // DEFAULT_HISTORY_PROVIDER_H_
