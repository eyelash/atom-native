#ifndef SCREEN_LINE_BUILDER_H_
#define SCREEN_LINE_BUILDER_H_

#include "display-layer.h"
#include "language-mode.h"

struct DisplayLayer;

struct ScreenLineBuilder {
  DisplayLayer *displayLayer;
  double requestedStartScreenRow;
  double requestedEndScreenRow;
  Point bufferPosition;
  double screenRow;
  std::vector<int32_t> containingScopeIds;
  std::vector<int32_t> scopeIdsToReopen;
  std::vector<DisplayLayer::ScreenLine> screenLines;
  int32_t currentBuiltInClassNameFlags;
  double bufferLineLength;
  double trailingWhitespaceStartColumn;
  bool inLeadingWhitespace;
  bool inTrailingWhitespace;
  std::u16string currentScreenLineText;
  std::vector<int32_t> currentScreenLineTags;
  double screenColumn;
  int32_t currentTokenLength;
  bool emitBuiltInTagBoundary;

  ScreenLineBuilder(DisplayLayer *);
  ~ScreenLineBuilder();

  std::vector<DisplayLayer::ScreenLine> buildScreenLines(double, double);
  double getBuiltInScopeId(int32_t);
  void beginLine();
  void updateCurrentTokenFlags(char16_t);
  void emitDecorationBoundaries(LanguageMode::HighlightIterator &);
  void emitFold(const Patch::Change &, LanguageMode::HighlightIterator &, double);
  void emitSoftWrap(const Patch::Change &);
  void emitLineEnding();
  void emitNewline(double = -1);
  void emitIndentWhitespace(double);
  void emitHardTab();
  void emitText(const std::u16string &, bool = true);
  void emitTokenBoundary();
  void emitEmptyTokenIfNeeded();
  void emitCloseTag(int32_t);
  void emitOpenTag(double, bool = true);
  void closeContainingScopes();
  void reopenTags();
  void pushScreenLine(const DisplayLayer::ScreenLine &);
  double compareBufferPosition(const Point &);
};

#endif // SCREEN_LINE_BUILDER_H_
