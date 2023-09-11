#include <catch.hpp>
#include <text-buffer.h>
#include <text-editor.h>
#include <cursor.h>

TEST_CASE("TextEditor") {
  TextBuffer *buffer = TextBuffer::loadSync(FIXTURES "/sample.js");
  TextEditor *editor = new TextEditor(buffer);
  std::vector<double> lineLengths;
  for (const std::u16string &line : buffer->getLines()) {
    lineLengths.push_back(line.size());
  }

  SECTION("cursor") {
    SECTION(".getLastCursor()") {
      SECTION("returns the most recently created cursor") {
        editor->addCursorAtScreenPosition({1, 0});
        Cursor *lastCursor = editor->addCursorAtScreenPosition({2, 0});
        REQUIRE(editor->getLastCursor() == lastCursor);
      }

      SECTION("creates a new cursor at (0, 0) if the last cursor has been destroyed") {
        editor->getLastCursor()->destroy();
        REQUIRE(editor->getLastCursor()->getBufferPosition() == Point(0, 0));
      }
    }

    SECTION(".getCursors()") {
      SECTION("creates a new cursor at (0, 0) if the last cursor has been destroyed") {
        editor->getLastCursor()->destroy();
        REQUIRE(editor->getCursors()[0]->getBufferPosition() == Point(0, 0));
      }
    }

    SECTION("when the cursor moves") {
      SECTION("clears a goal column established by vertical movement") {
        editor->setText(u"b");
        editor->setCursorBufferPosition({0, 0});
        editor->insertNewline();
        editor->moveUp();
        editor->insertText(u"a");
        editor->moveDown();
        REQUIRE(editor->getCursorBufferPosition() == Point(1, 1));
      }
    }

    SECTION(".setCursorScreenPosition(screenPosition)") {
      SECTION("clears a goal column established by vertical movement") {
        // set a goal column by moving down
        editor->setCursorScreenPosition({ 3, lineLengths[3] });
        editor->moveDown();
        REQUIRE(editor->getCursorScreenPosition().column != 6);

        // clear the goal column by explicitly setting the cursor position
        editor->setCursorScreenPosition({4, 6});
        REQUIRE(editor->getCursorScreenPosition().column == 6);

        editor->moveDown();
        REQUIRE(editor->getCursorScreenPosition().column == 6);
      }

      SECTION("merges multiple cursors") {
        editor->setCursorScreenPosition({0, 0});
        editor->addCursorAtScreenPosition({0, 1});
        Cursor *cursor1 = editor->getCursors()[0];
        editor->setCursorScreenPosition({4, 7});
        REQUIRE(editor->getCursors().size() == 1);
        REQUIRE(editor->getCursors()[0] == cursor1);
        REQUIRE(editor->getCursorScreenPosition() == Point(4, 7));
      }
    }
  }

  SECTION(".transpose()") {
    SECTION("swaps two characters") {
      editor->buffer->setText(u"abc");
      editor->setCursorScreenPosition({0, 1});
      editor->transpose();
      REQUIRE(editor->lineTextForBufferRow(0) == u"bac");
    }

    SECTION("reverses a selection") {
      editor->buffer->setText(u"xabcz");
      editor->setSelectedBufferRange({{0, 1}, {0, 4}});
      editor->transpose();
      REQUIRE(editor->lineTextForBufferRow(0) == u"xcbaz");
    }
  }

  SECTION(".upperCase()") {
    SECTION("when there is no selection") {
      SECTION("upper cases the current word") {
        editor->buffer->setText(u"aBc");
        editor->setCursorScreenPosition({0, 1});
        editor->upperCase();
        REQUIRE(editor->lineTextForBufferRow(0) == u"ABC");
        REQUIRE(editor->getSelectedBufferRange() == Range(Point(0, 0), Point(0, 3)));
      }
    }

    SECTION("when there is a selection") {
      SECTION("upper cases the current selection") {
        editor->buffer->setText(u"abc");
        editor->setSelectedBufferRange({{0, 0}, {0, 2}});
        editor->upperCase();
        REQUIRE(editor->lineTextForBufferRow(0) == u"ABc");
        REQUIRE(editor->getSelectedBufferRange() == Range(Point(0, 0), Point(0, 2)));
      }
    }
  }

  SECTION(".lowerCase()") {
    SECTION("when there is no selection") {
      SECTION("lower cases the current word") {
        editor->buffer->setText(u"aBC");
        editor->setCursorScreenPosition({0, 1});
        editor->lowerCase();
        REQUIRE(editor->lineTextForBufferRow(0) == u"abc");
        REQUIRE(editor->getSelectedBufferRange() == Range(Point(0, 0), Point(0, 3)));
      }
    }

    SECTION("when there is a selection") {
      SECTION("lower cases the current selection") {
        editor->buffer->setText(u"ABC");
        editor->setSelectedBufferRange({{0, 0}, {0, 2}});
        editor->lowerCase();
        REQUIRE(editor->lineTextForBufferRow(0) == u"abC");
        REQUIRE(editor->getSelectedBufferRange() == Range(Point(0, 0), Point(0, 2)));
      }
    }
  }

  delete editor;
}
