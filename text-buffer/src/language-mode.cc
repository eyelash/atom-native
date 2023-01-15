#include "language-mode.h"

LanguageMode::HighlightIterator::~HighlightIterator() {}

std::vector<int32_t> LanguageMode::HighlightIterator::seek(Point position, double endRow) {
	return std::vector<int32_t>();
}

bool LanguageMode::HighlightIterator::moveToSuccessor() {
  return false;
}

Point LanguageMode::HighlightIterator::getPosition() {
  return Point::INFINITY_;
}

std::vector<int32_t> LanguageMode::HighlightIterator::getCloseTags() {
  return std::vector<int32_t>();
}

std::vector<int32_t> LanguageMode::HighlightIterator::getOpenTags() {
  return std::vector<int32_t>();
}

std::vector<int32_t> LanguageMode::HighlightIterator::getCloseScopeIds() {
  return std::vector<int32_t>();
}

std::vector<int32_t> LanguageMode::HighlightIterator::getOpenScopeIds() {
  return std::vector<int32_t>();
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
