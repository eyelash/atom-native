#include "language-mode.h"

static const std::vector<int32_t> EMPTY;

LanguageMode::HighlightIterator::~HighlightIterator() {}

std::vector<int32_t> LanguageMode::HighlightIterator::seek(const Point &position, double) {
  return EMPTY;
}

void LanguageMode::HighlightIterator::moveToSuccessor() {}

Point LanguageMode::HighlightIterator::getPosition() {
  return Point::INFINITY_;
}

std::vector<int32_t> LanguageMode::HighlightIterator::getCloseScopeIds() {
  return EMPTY;
}

std::vector<int32_t> LanguageMode::HighlightIterator::getOpenScopeIds() {
  return EMPTY;
}

LanguageMode::~LanguageMode() {}

void LanguageMode::bufferDidChange(const Range &, const Range &, const std::u16string &, const std::u16string &) {}

void LanguageMode::bufferDidFinishTransaction() {}

std::unique_ptr<LanguageMode::HighlightIterator> LanguageMode::buildHighlightIterator() {
  return std::unique_ptr<HighlightIterator>(new HighlightIterator());
}

void LanguageMode::onDidChangeHighlighting(std::function<void(const Range &)>) {}

std::string LanguageMode::classNameForScopeId(int32_t) {
  return std::string();
}

bool LanguageMode::isRowCommented(double) {
  return false;
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

Grammar *LanguageMode::getGrammar() {
  return nullptr;
}
