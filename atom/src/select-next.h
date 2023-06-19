#ifndef SELECT_NEXT_H_
#define SELECT_NEXT_H_

#include <text-buffer.h>

struct TextEditor;
struct Selection;

struct SelectNext {
  TextEditor *editor;
  bool wordSelected;

  SelectNext(TextEditor *);
  ~SelectNext();

  void findAndSelectNext();
  void selectWord();
  void selectNextOccurrence();
  optional<Range> findNextOccurrence(Range);
  void addSelection(Range);
  void scanForNextOccurrence(Range, TextBuffer::ScanIterator);
  bool isNonWordCharacter(const std::u16string &);
  bool isNonWordCharacterToTheLeft(Selection *);
  bool isNonWordCharacterToTheRight(Selection *);
  bool isWordSelected(Selection *);
};

#endif // SELECT_NEXT_H_
