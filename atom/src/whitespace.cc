#include "whitespace.h"
#include "text-editor.h"
#include "cursor.h"

static const Regex TRAILING_WHITESPACE_REGEX = Regex(u"[ \t]+(?=\r?$)");

Whitespace::Whitespace() {}

Whitespace::~Whitespace() {}

void Whitespace::handleEvents(TextEditor *editor) {
  TextBuffer *buffer = editor->getBuffer();

  buffer->onWillSave([this, editor, buffer]() {
    return buffer->transact([&]() {
      this->removeTrailingWhitespace(editor);

      this->ensureSingleTrailingNewline(editor);
    });
  });

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

void Whitespace::removeTrailingWhitespace(TextEditor *editor) {
  TextBuffer *buffer = editor->getBuffer();

  // When buffer is same buffer of activeEditor's buffer, don't remove
  // trailing WS at activeEditor's cursor line.
  std::unordered_set<double> cursorRows;
  for (Cursor *cursor : editor->getCursors()) {
    cursorRows.insert(cursor->getBufferRow());
  }

  const bool ignoreCurrentLine = true;

  const bool ignoreWhitespaceOnlyLines = false;

  /*const keepMarkdownLineBreakWhitespace =
    grammarScopeName === ('source.gfm' || 'text.md') &&
    atom.config.get('whitespace.keepMarkdownLineBreakWhitespace')*/

  buffer->transact([&]() {
    const auto ranges = buffer->findAllSync(TRAILING_WHITESPACE_REGEX);
    for (size_t i = 0, n = ranges.size(); i < n; i++) {
      const Range range = ranges[i];
      const double row = range.start.row;
      const double trailingWhitespaceStart = ranges[i].start.column;
      if (ignoreCurrentLine && cursorRows.count(row)) continue;
      if (ignoreWhitespaceOnlyLines && trailingWhitespaceStart == 0) continue;
      /*if (keepMarkdownLineBreakWhitespace) {
        const whitespaceLength = range.end.column - range.start.column
        if (trailingWhitespaceStart > 0 && whitespaceLength >= 2) continue
      }*/
      buffer->delete_(ranges[i]);
    }
  });
}

void Whitespace::ensureSingleTrailingNewline(TextEditor *editor) {
  std::vector<Range> selectedBufferRanges;
  double row;
  TextBuffer *buffer = editor->getBuffer();
  double lastRow = buffer->getLastRow();

  if (buffer->lineForRow(lastRow) == u"") {
    row = lastRow - 1;

    while (row && buffer->lineForRow(row) == u"") {
      buffer->deleteRow(row--);
    }
  } else {
    selectedBufferRanges = editor->getSelectedBufferRanges();
    buffer->append(u"\n");
    editor->setSelectedBufferRanges(selectedBufferRanges);
  }
}
