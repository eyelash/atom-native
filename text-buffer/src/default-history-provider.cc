#include "default-history-provider.h"
#include <chrono>

DefaultHistoryProvider::Checkpoint::Checkpoint(unsigned id, const TextBuffer::MarkerSnapshot &snapshot, bool isBarrier) :
  id{id},
  snapshot{snapshot},
  isBarrier{isBarrier} {}

static double now() {
  return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(std::chrono::system_clock::now().time_since_epoch()).count();
}

DefaultHistoryProvider::Transaction::Transaction(const TextBuffer::MarkerSnapshot &markerSnapshotBefore, Patch &&patch, const TextBuffer::MarkerSnapshot &markerSnapshotAfter, double groupingInterval) :
  markerSnapshotBefore{markerSnapshotBefore},
  patch{std::move(patch)},
  markerSnapshotAfter{markerSnapshotAfter},
  groupingInterval{groupingInterval},
  timestamp{now()} {}

bool DefaultHistoryProvider::Transaction::shouldGroupWith(Transaction *previousTransaction) {
  const double timeBetweenTransactions = this->timestamp - previousTransaction->timestamp;
  return timeBetweenTransactions < std::min(this->groupingInterval, previousTransaction->groupingInterval);
}

DefaultHistoryProvider::Transaction *DefaultHistoryProvider::Transaction::groupWith(Transaction *previousTransaction) {
  return new Transaction(previousTransaction->markerSnapshotBefore, Patch::compose({&previousTransaction->patch, &this->patch}), this->markerSnapshotAfter, this->groupingInterval);
}

DefaultHistoryProvider::StackEntry::StackEntry(Checkpoint *value) {
  this->value = value;
  this->type = Type::Checkpoint;
}

DefaultHistoryProvider::StackEntry::StackEntry(Transaction *value) {
  this->value = value;
  this->type = Type::Transaction;
}

DefaultHistoryProvider::StackEntry::StackEntry(Patch *value) {
  this->value = value;
  this->type = Type::Patch;
}

DefaultHistoryProvider::StackEntry::StackEntry(StackEntry &&entry) {
  this->value = entry.value;
  this->type = entry.type;
  entry.value = nullptr;
}

DefaultHistoryProvider::StackEntry::~StackEntry() {
  if (this->value) {
    switch (this->type) {
      case Type::Checkpoint:
        delete static_cast<Checkpoint *>(this->value);
        break;
      case Type::Transaction:
        delete static_cast<Transaction *>(this->value);
        break;
      case Type::Patch:
        delete static_cast<Patch *>(this->value);
        break;
    }
  }
}

DefaultHistoryProvider::StackEntry &DefaultHistoryProvider::StackEntry::operator=(StackEntry &&entry) {
  std::swap(this->value, entry.value);
  std::swap(this->type, entry.type);
  return *this;
}

DefaultHistoryProvider::Result::Result() {}

DefaultHistoryProvider::Result::Result(Patch &&patch, TextBuffer::MarkerSnapshot &&markers) :
  patch{std::move(patch)},
  markers{std::move(markers)} {}

DefaultHistoryProvider::Result::operator bool() const {
  return patch.get_change_count() > 0 || markers.size() > 0;
}

std::vector<Patch::Change> DefaultHistoryProvider::Result::textUpdates() const {
  return patch.get_changes();
}

DefaultHistoryProvider::DefaultHistoryProvider() {
  this->maxUndoEntries = 10000;
  this->nextCheckpointId = 1;
}

DefaultHistoryProvider::~DefaultHistoryProvider() {}

unsigned DefaultHistoryProvider::createCheckpoint(const TextBuffer::MarkerSnapshot &markers, bool isBarrier) {
  Checkpoint *checkpoint = new Checkpoint(this->nextCheckpointId++, markers, isBarrier);
  this->undoStack.push_back(checkpoint);
  return checkpoint->id;
}

void DefaultHistoryProvider::groupChangesSinceCheckpoint(unsigned checkpointId, const TextBuffer::MarkerSnapshot &markerSnapshotAfter, bool deleteCheckpoint) {
  optional<double> checkpointIndex;
  TextBuffer::MarkerSnapshot markerSnapshotBefore;
  std::vector<Patch *> patchesSinceCheckpoint;
  for (double i = this->undoStack.size() - 1.0; i >= 0; i--) {
    const StackEntry &entry = this->undoStack[i];
    if (checkpointIndex) {
      break;
    }
    switch (entry.type) {
      case StackEntry::Type::Checkpoint:
        {
          Checkpoint *checkpoint = static_cast<Checkpoint *>(entry.value);
          if (checkpoint->id == checkpointId) {
            checkpointIndex = i;
            markerSnapshotBefore = checkpoint->snapshot;
          } else if (checkpoint->isBarrier) {
            return;
          }
        }
        break;
      case StackEntry::Type::Transaction:
        patchesSinceCheckpoint.insert(patchesSinceCheckpoint.begin(), &static_cast<Transaction *>(entry.value)->patch);
        break;
      case StackEntry::Type::Patch:
        patchesSinceCheckpoint.insert(patchesSinceCheckpoint.begin(), static_cast<Patch *>(entry.value));
        break;
    }
  }
  if (checkpointIndex) {
    if (patchesSinceCheckpoint.size() > 0) {
      Patch composedPatches = Patch::compose(patchesSinceCheckpoint);
      this->undoStack.erase(this->undoStack.begin() + *checkpointIndex + 1, this->undoStack.end());
      this->undoStack.push_back(new Transaction(markerSnapshotBefore, std::move(composedPatches), markerSnapshotAfter));
    }
    if (deleteCheckpoint) {
      this->undoStack.erase(this->undoStack.begin() + *checkpointIndex);
    }
  }
}

void DefaultHistoryProvider::enforceUndoStackSizeLimit() {
  if (this->undoStack.size() > this->maxUndoEntries) {
    this->undoStack.erase(this->undoStack.begin(), this->undoStack.end() - this->maxUndoEntries);
  }
}

void DefaultHistoryProvider::applyGroupingInterval(double groupingInterval) {
  StackEntry *topEntry = this->undoStack.size() >= 1 ? &this->undoStack[this->undoStack.size() - 1] : nullptr;
  StackEntry *previousEntry = this->undoStack.size() >= 2 ? &this->undoStack[this->undoStack.size() - 2] : nullptr;
  if (topEntry && topEntry->type == StackEntry::Type::Transaction) {
    static_cast<Transaction *>(topEntry->value)->groupingInterval = groupingInterval;
  } else {
    return;
  }
  if (groupingInterval == 0) {
    return;
  }
  if (previousEntry && previousEntry->type == StackEntry::Type::Transaction && static_cast<Transaction *>(topEntry->value)->shouldGroupWith(static_cast<Transaction *>(previousEntry->value))) {
    this->undoStack[this->undoStack.size() - 2] = static_cast<Transaction *>(topEntry->value)->groupWith(static_cast<Transaction *>(previousEntry->value));
    this->undoStack.pop_back();
  }
}

void DefaultHistoryProvider::pushChange(Point newStart, Point oldExtent, Point newExtent, const std::u16string &oldText, const std::u16string &newText) {
  Patch *patch = new Patch();
  patch->splice(newStart, oldExtent, newExtent, Text(oldText), Text(newText));
  this->pushPatch(patch);
}

void DefaultHistoryProvider::pushPatch(Patch *patch) {
  this->undoStack.push_back(patch);
  this->clearRedoStack();
}

DefaultHistoryProvider::Result DefaultHistoryProvider::undo() {
  TextBuffer::MarkerSnapshot snapshotBelow;
  Patch patch;
  optional<double> spliceIndex;
  for (double i = this->undoStack.size() - 1.0; i >= 0; i--) {
    const StackEntry &entry = this->undoStack[i];
    if (spliceIndex) {
      break;
    }
    switch (entry.type) {
      case StackEntry::Type::Checkpoint:
        if (static_cast<Checkpoint *>(entry.value)->isBarrier) {
          return Result();
        }
        break;
      case StackEntry::Type::Transaction:
        snapshotBelow = static_cast<Transaction *>(entry.value)->markerSnapshotBefore;
        patch = static_cast<Transaction *>(entry.value)->patch.invert();
        spliceIndex = i;
        break;
      case StackEntry::Type::Patch:
        patch = static_cast<Patch *>(entry.value)->invert();
        spliceIndex = i;
        break;
    }
  }
  if (spliceIndex) {
    std::vector<StackEntry> entries(std::make_move_iterator(this->undoStack.begin() + *spliceIndex), std::make_move_iterator(this->undoStack.end()));
    this->undoStack.erase(this->undoStack.begin() + *spliceIndex, this->undoStack.end());
    this->redoStack.insert(this->redoStack.end(), std::make_move_iterator(entries.rbegin()), std::make_move_iterator(entries.rend()));
  }
  return Result(std::move(patch), std::move(snapshotBelow));
}

DefaultHistoryProvider::Result DefaultHistoryProvider::redo() {
  TextBuffer::MarkerSnapshot snapshotBelow;
  Patch patch;
  optional<double> spliceIndex;
  for (double i = this->redoStack.size() - 1.0; i >= 0; i--) {
    const StackEntry &entry = this->redoStack[i];
    if (spliceIndex) {
      break;
    }
    switch (entry.type) {
      case StackEntry::Type::Checkpoint:
        if (static_cast<Checkpoint *>(entry.value)->isBarrier) {
          //throw new Error("Invalid redo stack state");
        }
        break;
      case StackEntry::Type::Transaction:
        snapshotBelow = static_cast<Transaction *>(entry.value)->markerSnapshotAfter;
        patch = static_cast<Transaction *>(entry.value)->patch.copy();
        spliceIndex = i;
        break;
      case StackEntry::Type::Patch:
        patch = static_cast<Patch *>(entry.value)->copy();
        spliceIndex = i;
        break;
    }
  }
  if (spliceIndex) {
    while (*spliceIndex - 1 >= 0 && this->redoStack[*spliceIndex - 1].type == StackEntry::Type::Checkpoint) {
      spliceIndex = *spliceIndex - 1;
    }
    std::vector<StackEntry> entries(std::make_move_iterator(this->redoStack.begin() + *spliceIndex), std::make_move_iterator(this->redoStack.end()));
    this->redoStack.erase(this->redoStack.begin() + *spliceIndex, this->redoStack.end());
    this->undoStack.insert(this->undoStack.end(), std::make_move_iterator(entries.rbegin()), std::make_move_iterator(entries.rend()));
  }
  return Result(std::move(patch), std::move(snapshotBelow));
}

void DefaultHistoryProvider::clear() {
  this->clearUndoStack();
  this->clearRedoStack();
}

void DefaultHistoryProvider::clearUndoStack() {
  this->undoStack.clear();
}

void DefaultHistoryProvider::clearRedoStack() {
  this->redoStack.clear();
}
