#include "whitespace.h"
#include "text-editor.h"

Whitespace::Whitespace() {}

Whitespace::~Whitespace() {}

void Whitespace::handleEvents(TextEditor *editor) {
  TextBuffer *buffer = editor->getBuffer();

  editor->onDidInsertText([editor, buffer](const std::u16string &text, const Range &range) {
    if (text != u"\n") {
      return;
    }

    if (!buffer->isRowBlank(range.start.row)) {
      return;
    }

    editor->setIndentationForBufferRow(range.start.row, 0);
  });
}
