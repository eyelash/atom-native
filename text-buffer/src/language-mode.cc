#include "language-mode.h"

static const std::vector<int32_t> EMPTY;

LanguageMode::HighlightIterator::~HighlightIterator() {}

std::vector<int32_t> LanguageMode::HighlightIterator::seek(Point position, double endRow) {
	return EMPTY;
}

void LanguageMode::HighlightIterator::moveToSuccessor() {}

Point LanguageMode::HighlightIterator::getPosition() {
  return Point::INFINITY_;
}

std::vector<int32_t> LanguageMode::HighlightIterator::getCloseTags() {
  return EMPTY;
}

std::vector<int32_t> LanguageMode::HighlightIterator::getOpenTags() {
  return EMPTY;
}

std::vector<int32_t> LanguageMode::HighlightIterator::getCloseScopeIds() {
  return EMPTY;
}

std::vector<int32_t> LanguageMode::HighlightIterator::getOpenScopeIds() {
  return EMPTY;
}

LanguageMode::~LanguageMode() {}

void LanguageMode::bufferDidChange(Range, Range, const std::u16string &, const std::u16string &) {}

void LanguageMode::bufferDidFinishTransaction() {}

std::unique_ptr<LanguageMode::HighlightIterator> LanguageMode::buildHighlightIterator() {
  return std::unique_ptr<HighlightIterator>(new HighlightIterator());
}

void LanguageMode::onDidChangeHighlighting(std::function<void(Range)>) {}

std::string LanguageMode::classNameForScopeId(int32_t scopeId) {
  return std::string();
}

std::u16string LanguageMode::getLanguageId() {
  return std::u16string();
}

double LanguageMode::suggestedIndentForLineAtBufferRow(double, const std::u16string &, double) {
  return 0;
}

double LanguageMode::suggestedIndentForBufferRow(double, double, bool) {
  return 0;
}

optional<double> LanguageMode::suggestedIndentForEditedBufferRow(double, double) {
  return optional<double>();
}

optional<Range> LanguageMode::getRangeForSyntaxNodeContainingRange(Range) {
  return optional<Range>();
}
