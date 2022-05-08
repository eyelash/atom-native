#include "selection.h"
#include "text-editor.h"
#include <display-marker.h>
#include <text-buffer.h>

Selection::Selection(TextEditor *editor, DisplayMarker *marker, Cursor *cursor) {
  this->editor = editor;
  this->marker = marker;
  this->cursor = cursor;
  this->marker->onDidDestroy([this]() { this->markerDidDestroy(); });
}

Selection::~Selection() {}

void Selection::destroy() {
  this->marker->destroy();
}

/*
Section: Event Subscription
*/

/*
Section: Managing the selection range
*/

Range Selection::getScreenRange() {
  return this->marker->getScreenRange();
}

Range Selection::getBufferRange() {
  return this->marker->getBufferRange();
}

/*
Section: Info about the selection
*/

bool Selection::isReversed() {
  return this->marker->isReversed();
}

/*
Section: Modifying the selected range
*/

/*
Section: Modifying the selected text
*/

void Selection::insertText(const std::u16string &text) {
  const Range oldBufferRange = this->getBufferRange();

  this->editor->getBuffer()->setTextInRange(
    oldBufferRange,
    text
  );
}

/*
Section: Managing multiple selections
*/

/*
Section: Comparing to other selections
*/

int Selection::compare(Selection *otherSelection) {
  return this->marker->compare(otherSelection->marker);
}

/*
Section: Private Utilities
*/

void Selection::markerDidDestroy() {
  this->editor->removeSelection(this);
}
