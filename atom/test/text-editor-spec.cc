#include <catch.hpp>
#include <text-buffer.h>
#include <text-editor.h>
#include <cursor.h>
#include <selection.h>

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

    SECTION(".moveUp()") {
      SECTION("moves the cursor up") {
        editor->setCursorScreenPosition({2, 2});
        editor->moveUp();
        REQUIRE(editor->getCursorScreenPosition() == Point(1, 2));
      }

      SECTION("retains the goal column across lines of differing length") {
        REQUIRE(lineLengths[6] > 32);
        editor->setCursorScreenPosition({ 6, 32 });

        editor->moveUp();
        REQUIRE(editor->getCursorScreenPosition().column == lineLengths[5]);

        editor->moveUp();
        REQUIRE(editor->getCursorScreenPosition().column == lineLengths[4]);

        editor->moveUp();
        REQUIRE(editor->getCursorScreenPosition().column == 32);
      }

      SECTION("when the cursor is on the first line") {
        SECTION("moves the cursor to the beginning of the line, but retains the goal column") {
          editor->setCursorScreenPosition({0, 4});
          editor->moveUp();
          REQUIRE(editor->getCursorScreenPosition() == Point(0, 0));

          editor->moveDown();
          REQUIRE(editor->getCursorScreenPosition() == Point(1, 4));
        }
      }

      SECTION("when there is a selection") {
        editor->setSelectedBufferRange({{4, 9}, {5, 10}});

        SECTION("moves above the selection") {
          Cursor *cursor = editor->getLastCursor();
          editor->moveUp();
          REQUIRE(cursor->getBufferPosition() == Point(3, 9));
        }
      }
    }

    SECTION(".moveDown()") {
      SECTION("moves the cursor down") {
        editor->setCursorScreenPosition({2, 2});
        editor->moveDown();
        REQUIRE(editor->getCursorScreenPosition() == Point(3, 2));
      }

      SECTION("retains the goal column across lines of differing length") {
        editor->setCursorScreenPosition({ 3, lineLengths[3] });

        editor->moveDown();
        REQUIRE(editor->getCursorScreenPosition().column == lineLengths[4]);

        editor->moveDown();
        REQUIRE(editor->getCursorScreenPosition().column == lineLengths[5]);

        editor->moveDown();
        REQUIRE(editor->getCursorScreenPosition().column == lineLengths[3]);
      }

      SECTION("when there is a selection") {
        editor->setSelectedBufferRange({{4, 9}, {5, 10}});

        SECTION("moves below the selection") {
          Cursor *cursor = editor->getLastCursor();
          editor->moveDown();
          REQUIRE(cursor->getBufferPosition() == Point(6, 10));
        }
      }
    }

    SECTION(".moveLeft()") {
      SECTION("moves the cursor by one column to the left") {
        editor->setCursorScreenPosition({1, 8});
        editor->moveLeft();
        REQUIRE(editor->getCursorScreenPosition() == Point(1, 7));
      }

      SECTION("moves the cursor by n columns to the left") {
        editor->setCursorScreenPosition({1, 8});
        editor->moveLeft(4);
        REQUIRE(editor->getCursorScreenPosition() == Point(1, 4));
      }

      SECTION("moves the cursor by two rows up when the columnCount is longer than an entire line") {
        editor->setCursorScreenPosition({2, 2});
        editor->moveLeft(34);
        REQUIRE(editor->getCursorScreenPosition() == Point(0, 29));
      }

      SECTION("moves the cursor to the beginning columnCount is longer than the position in the buffer") {
        editor->setCursorScreenPosition({1, 0});
        editor->moveLeft(100);
        REQUIRE(editor->getCursorScreenPosition() == Point(0, 0));
      }

      SECTION("when the cursor is in the first column") {
        SECTION("when there is a previous line") {
          SECTION("wraps to the end of the previous line") {
            editor->setCursorScreenPosition({ 1, 0 });
            editor->moveLeft();
            REQUIRE(editor->getCursorScreenPosition() == Point(
              0,
              buffer->lineForRow(0).size()
            ));
          }

          SECTION("moves the cursor by one row up and n columns to the left") {
            editor->setCursorScreenPosition({1, 0});
            editor->moveLeft(4);
            REQUIRE(editor->getCursorScreenPosition() == Point(0, 26));
          }
        }

        SECTION("when the next line is empty") {
          SECTION("wraps to the beginning of the previous line") {
            editor->setCursorScreenPosition({11, 0});
            editor->moveLeft();
            REQUIRE(editor->getCursorScreenPosition() == Point(10, 0));
          }
        }

        SECTION("when the cursor is on the first line") {
          SECTION("remains in the same position (0,0)") {
            editor->setCursorScreenPosition({ 0, 0 });
            editor->moveLeft();
            REQUIRE(editor->getCursorScreenPosition() == Point(0, 0));
          }

          SECTION("remains in the same position (0,0) when columnCount is specified") {
            editor->setCursorScreenPosition({0, 0});
            editor->moveLeft(4);
            REQUIRE(editor->getCursorScreenPosition() == Point(0, 0));
          }
        }
      }

      SECTION("when there is a selection") {
        editor->setSelectedBufferRange({{5, 22}, {5, 27}});

        SECTION("moves to the left of the selection") {
          Cursor *cursor = editor->getLastCursor();
          editor->moveLeft();
          REQUIRE(cursor->getBufferPosition() == Point(5, 22));

          editor->moveLeft();
          REQUIRE(cursor->getBufferPosition() == Point(5, 21));
        }
      }
    }

    SECTION(".moveRight()") {
      SECTION("moves the cursor by one column to the right") {
        editor->setCursorScreenPosition({3, 3});
        editor->moveRight();
        REQUIRE(editor->getCursorScreenPosition() == Point(3, 4));
      }

      SECTION("moves the cursor by n columns to the right") {
        editor->setCursorScreenPosition({3, 7});
        editor->moveRight(4);
        REQUIRE(editor->getCursorScreenPosition() == Point(3, 11));
      }

      SECTION("moves the cursor by two rows down when the columnCount is longer than an entire line") {
        editor->setCursorScreenPosition({0, 29});
        editor->moveRight(34);
        REQUIRE(editor->getCursorScreenPosition() == Point(2, 2));
      }

      SECTION("moves the cursor to the end of the buffer when columnCount is longer than the number of characters following the cursor position") {
        editor->setCursorScreenPosition({11, 5});
        editor->moveRight(100);
        REQUIRE(editor->getCursorScreenPosition() == Point(12, 2));
      }

      SECTION("when there is a selection") {
        editor->setSelectedBufferRange({{5, 22}, {5, 27}});

        SECTION("moves to the left of the selection") {
          Cursor *cursor = editor->getLastCursor();
          editor->moveRight();
          REQUIRE(cursor->getBufferPosition() == Point(5, 27));

          editor->moveRight();
          REQUIRE(cursor->getBufferPosition() == Point(5, 28));
        }
      }
    }

    SECTION("getCursorAtScreenPosition(screenPosition)") {
      SECTION("returns the cursor at the given screenPosition") {
        Cursor *cursor1 = editor->addCursorAtScreenPosition({0, 2});
        Cursor *cursor2 = editor->getCursorAtScreenPosition(
          cursor1->getScreenPosition()
        );
        REQUIRE(cursor2 == cursor1);
      }
    }

    SECTION("addCursorAtScreenPosition(screenPosition)") {
      SECTION("when a cursor already exists at the position") {
        SECTION("returns the existing cursor") {
          Cursor *cursor1 = editor->addCursorAtScreenPosition({0, 2});
          Cursor *cursor2 = editor->addCursorAtScreenPosition({0, 2});
          REQUIRE(cursor2 == cursor1);
        }
      }
    }

    SECTION("addCursorAtBufferPosition(bufferPosition)") {
      SECTION("when a cursor already exists at the position") {
        SECTION("returns the existing cursor") {
          Cursor *cursor1 = editor->addCursorAtBufferPosition({1, 4});
          Cursor *cursor2 = editor->addCursorAtBufferPosition({1, 4});
          REQUIRE(cursor2->marker == cursor1->marker);
        }
      }
    }
  }

  SECTION("selection") {}

  SECTION("buffer manipulation") {
    SECTION(".insertText(text)") {
      SECTION("when there is a single selection") {
        editor->setSelectedBufferRange({{1, 0}, {1, 2}});

        SECTION("replaces the selection with the given text") {
          //const Range range = editor->insertText(u"xxx");
          //REQUIRE(range == Range(Point(1, 0), Point(1, 3)));
          //REQUIRE(buffer->lineForRow(1) == u"xxxvar sort = function(items) {");
        }
      }

      SECTION("when there are multiple empty selections") {
        SECTION("when the cursors are on the same line") {
          SECTION("inserts the given text at the location of each cursor and moves the cursors to the end of each cursor's inserted text") {
            editor->setCursorScreenPosition({1, 2});
            editor->addCursorAtScreenPosition({1, 5});

            editor->insertText(u"xxx");

            REQUIRE(buffer->lineForRow(1) ==
              u"  xxxvarxxx sort = function(items) {"
            );
            const auto cursors = editor->getCursors();

            REQUIRE(cursors.size() == 2);
            REQUIRE(cursors[0]->getBufferPosition() == Point(1, 5));
            REQUIRE(cursors[1]->getBufferPosition() == Point(1, 11));
          }
        }

        SECTION("when the cursors are on different lines") {
          SECTION("inserts the given text at the location of each cursor and moves the cursors to the end of each cursor's inserted text") {
            editor->setCursorScreenPosition({1, 2});
            editor->addCursorAtScreenPosition({2, 4});

            editor->insertText(u"xxx");

            REQUIRE(buffer->lineForRow(1) ==
              u"  xxxvar sort = function(items) {"
            );
            REQUIRE(buffer->lineForRow(2) ==
              u"    xxxif (items.length <= 1) return items;"
            );
            const auto cursors = editor->getCursors();

            REQUIRE(cursors.size() == 2);
            REQUIRE(cursors[0]->getBufferPosition() == Point(1, 5));
            REQUIRE(cursors[1]->getBufferPosition() == Point(2, 7));
          }
        }
      }

      SECTION("when there are multiple non-empty selections") {
        SECTION("when the selections are on the same line") {
          SECTION("replaces each selection range with the inserted characters") {
            editor->setSelectedBufferRanges({
              {{0, 4}, {0, 13}},
              {{0, 22}, {0, 24}}
            });
            editor->insertText(u"x");

            const auto cursors = editor->getCursors();
            const auto selections = editor->getSelections();

            REQUIRE(cursors.size() == 2);
            REQUIRE(selections.size() == 2);
            REQUIRE(cursors[0]->getScreenPosition() == Point(0, 5));
            REQUIRE(cursors[1]->getScreenPosition() == Point(0, 15));
            REQUIRE(selections[0]->isEmpty());
            REQUIRE(selections[1]->isEmpty());

            REQUIRE(editor->lineTextForBufferRow(0) == u"var x = functix () {");
          }
        }

        SECTION("when the selections are on different lines") {
          SECTION("replaces each selection with the given text, clears the selections, and places the cursor at the end of each selection's inserted text") {
            editor->setSelectedBufferRanges({
              {{1, 0}, {1, 2}},
              {{2, 0}, {2, 4}}
            });

            editor->insertText(u"xxx");

            REQUIRE(buffer->lineForRow(1) ==
              u"xxxvar sort = function(items) {"
            );
            REQUIRE(buffer->lineForRow(2) ==
              u"xxxif (items.length <= 1) return items;"
            );
            const auto selections = editor->getSelections();

            REQUIRE(selections.size() == 2);
            REQUIRE(selections[0]->isEmpty());
            REQUIRE(selections[0]->cursor->getBufferPosition() == Point(1, 3));
            REQUIRE(selections[1]->isEmpty());
            REQUIRE(selections[1]->cursor->getBufferPosition() == Point(2, 3));
          }
        }
      }

      /*SECTION("when the undo option is set to 'skip'") {
        SECTION("groups the change with the previous change for purposes of undo and redo") {
          editor->setSelectedBufferRanges([[[0, 0], [0, 0]], [[1, 0], [1, 0]]]);
          editor->insertText('x');
          editor->insertText('y', { undo: 'skip' });
          editor->undo();
          REQUIRE(buffer->lineForRow(0)).toBe('var quicksort = function () {');
          REQUIRE(buffer->lineForRow(1)).toBe('  var sort = function(items) {');
        }
      }*/
    }

    SECTION(".insertNewline()") {
      SECTION("when there is a single cursor") {
        SECTION("when the cursor is at the beginning of a line") {
          SECTION("inserts an empty line before it") {
            editor->setCursorScreenPosition({ 1, 0 });

            editor->insertNewline();

            REQUIRE(buffer->lineForRow(1) == u"");
            REQUIRE(editor->getCursorScreenPosition() == Point(
              2,
              0
            ));
          }
        }

        SECTION("when the cursor is in the middle of a line") {
          SECTION("splits the current line to form a new line") {
            editor->setCursorScreenPosition({ 1, 6 });
            const std::u16string originalLine = buffer->lineForRow(1);
            const std::u16string lineBelowOriginalLine = buffer->lineForRow(2);

            editor->insertNewline();

            REQUIRE(buffer->lineForRow(1) == originalLine.substr(0, 6));
            REQUIRE(buffer->lineForRow(2) == originalLine.substr(6));
            REQUIRE(buffer->lineForRow(3) == lineBelowOriginalLine);
            REQUIRE(editor->getCursorScreenPosition() == Point(
              2,
              0
            ));
          }
        }

        SECTION("when the cursor is on the end of a line") {
          SECTION("inserts an empty line after it") {
            editor->setCursorScreenPosition({
              1,
              static_cast<double>(buffer->lineForRow(1).size())
            });

            editor->insertNewline();

            REQUIRE(buffer->lineForRow(2) == u"");
            REQUIRE(editor->getCursorScreenPosition() == Point(
              2,
              0
            ));
          }
        }
      }

      SECTION("when there are multiple cursors") {
        SECTION("when the cursors are on the same line") {
          SECTION("breaks the line at the cursor locations") {
            editor->setCursorScreenPosition({3, 13});
            editor->addCursorAtScreenPosition({3, 38});

            editor->insertNewline();

            REQUIRE(editor->lineTextForBufferRow(3) == u"    var pivot");
            REQUIRE(editor->lineTextForBufferRow(4) ==
              u" = items.shift(), current"
            );
            REQUIRE(editor->lineTextForBufferRow(5) ==
              u", left = [], right = [];"
            );
            REQUIRE(editor->lineTextForBufferRow(6) ==
              u"    while(items.length > 0) {"
            );

            const auto cursors = editor->getCursors();
            REQUIRE(cursors.size() == 2);
            REQUIRE(cursors[0]->getBufferPosition() == Point(4, 0));
            REQUIRE(cursors[1]->getBufferPosition() == Point(5, 0));
          }
        }

        SECTION("when the cursors are on different lines") {
          SECTION("inserts newlines at each cursor location") {
            editor->setCursorScreenPosition({3, 0});
            editor->addCursorAtScreenPosition({6, 0});

            editor->insertText(u"\n");
            REQUIRE(editor->lineTextForBufferRow(3) == u"");
            REQUIRE(editor->lineTextForBufferRow(4) ==
              u"    var pivot = items.shift(), current, left = [], right = [];"
            );
            REQUIRE(editor->lineTextForBufferRow(5) ==
              u"    while(items.length > 0) {"
            );
            REQUIRE(editor->lineTextForBufferRow(6) ==
              u"      current = items.shift();"
            );
            REQUIRE(editor->lineTextForBufferRow(7) == u"");
            REQUIRE(editor->lineTextForBufferRow(8) ==
              u"      current < pivot ? left.push(current) : right.push(current);"
            );
            REQUIRE(editor->lineTextForBufferRow(9) == u"    }");

            const auto cursors = editor->getCursors();
            REQUIRE(cursors.size() == 2);
            REQUIRE(cursors[0]->getBufferPosition() == Point(4, 0));
            REQUIRE(cursors[1]->getBufferPosition() == Point(8, 0));
          }
        }
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
