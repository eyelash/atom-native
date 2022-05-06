#include "selection.h"
#include "text-editor.h"
#include <display-marker.h>
#include <text-buffer.h>

Selection::Selection(TextEditor *editor, DisplayMarker *marker, Cursor *cursor) {
  this->editor = editor;
  this->marker = marker;
  this->cursor = cursor;
}

Selection::~Selection() {}

Range Selection::getScreenRange() {
  return this->marker->getScreenRange();
}

Range Selection::getBufferRange() {
  return this->marker->getBufferRange();
}

void Selection::insertText(const std::u16string &text) {
  const Range oldBufferRange = this->getBufferRange();

  this->editor->getBuffer()->setTextInRange(
    oldBufferRange,
    text
  );
}

int Selection::compare(Selection *otherSelection) {
  return this->marker->compare(otherSelection->marker);
}
