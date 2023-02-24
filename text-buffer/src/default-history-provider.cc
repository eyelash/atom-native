#include "default-history-provider.h"

DefaultHistoryProvider::Checkpoint::Checkpoint(unsigned id, bool isBarrier) {
  this->id = id;
  this->isBarrier = isBarrier;
}

DefaultHistoryProvider::Transaction::Transaction(Patch &&patch): patch{std::move(patch)} {}

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

DefaultHistoryProvider::StackEntry::StackEntry(DefaultHistoryProvider::StackEntry &&entry) {
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

DefaultHistoryProvider::StackEntry &DefaultHistoryProvider::StackEntry::operator =(DefaultHistoryProvider::StackEntry &&entry) {
  std::swap(this->value, entry.value);
  std::swap(this->type, entry.type);
  return *this;
}

DefaultHistoryProvider::DefaultHistoryProvider() {
  this->nextCheckpointId = 1;
}

DefaultHistoryProvider::~DefaultHistoryProvider() {}

unsigned DefaultHistoryProvider::createCheckpoint(bool isBarrier) {
  Checkpoint *checkpoint = new Checkpoint(this->nextCheckpointId++, /* options != null ? options.markers : void 0, */ isBarrier);
  this->undoStack.push_back(checkpoint);
  return checkpoint->id;
}

void DefaultHistoryProvider::groupChangesSinceCheckpoint(unsigned checkpointId, bool deleteCheckpoint) {
  optional<double> checkpointIndex;
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
            //markerSnapshotBefore = checkpoint->snapshot;
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
    Patch composedPatches = Patch::compose(patchesSinceCheckpoint);
    if (patchesSinceCheckpoint.size() > 0) {
      this->undoStack.erase(this->undoStack.begin() + *checkpointIndex + 1, this->undoStack.end());
      this->undoStack.push_back(new Transaction(/* markerSnapshotBefore, */ std::move(composedPatches) /* , markerSnapshotAfter */));
    }
    if (deleteCheckpoint) {
      this->undoStack.erase(this->undoStack.begin() + checkpointIndex);
    }
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

Patch DefaultHistoryProvider::undo() {
  Patch patch;
  optional<double> spliceIndex;
  for (double i = this->undoStack.size() - 1.0; i >= 0; i--) {
    const StackEntry &entry = this->undoStack[i];
    if (spliceIndex) {
      break;
    }
    switch (entry.type) {
      case StackEntry::Type::Checkpoint:
        {
          Checkpoint *checkpoint = static_cast<Checkpoint *>(entry.value);
          if (checkpoint->isBarrier) {
            return patch;
          }
        }
        break;
      case StackEntry::Type::Transaction:
        //snapshotBelow = entry.markerSnapshotBefore;
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
  return patch;
}

Patch DefaultHistoryProvider::redo() {
  Patch patch;
  optional<double> spliceIndex;
  for (double i = this->redoStack.size() - 1.0; i >= 0; i--) {
    const StackEntry &entry = this->redoStack[i];
    if (spliceIndex) {
      break;
    }
    switch (entry.type) {
      case StackEntry::Type::Checkpoint:
        {
          Checkpoint *checkpoint = static_cast<Checkpoint *>(entry.value);
          if (checkpoint->isBarrier) {
            //throw new Error("Invalid redo stack state");
          }
        }
        break;
      case StackEntry::Type::Transaction:
        //snapshotBelow = entry.markerSnapshotAfter;
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
  return patch;
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
