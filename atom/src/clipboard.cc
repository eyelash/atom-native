#include "clipboard.h"

Clipboard::Metadata::Metadata(double indentBasis, bool fullLine) :
  indentBasis{indentBasis}, fullLine{fullLine} {}

Clipboard::Metadata::Metadata() {}

Clipboard::Clipboard() {
  this->metadata = nullptr;
}

Clipboard::~Clipboard() {
  if (this->metadata) delete this->metadata;
}

std::u16string Clipboard::md5(const std::u16string &text) {
  // TODO: MD5
  return text;
}

void Clipboard::write(const std::u16string &text, Metadata *metadata) {
  this->signatureForMetadata = this->md5(text);
  if (metadata != this->metadata) {
    if (this->metadata) delete this->metadata;
    this->metadata = metadata;
  }
  this->systemText = text;
}

std::u16string Clipboard::read() {
  return this->systemText;
}

std::pair<std::u16string, Clipboard::Metadata *> Clipboard::readWithMetadata() {
  const std::u16string text = this->read();
  if (this->signatureForMetadata == this->md5(text)) {
    return { text, this->metadata };
  } else {
    return { text, nullptr };
  }
}
