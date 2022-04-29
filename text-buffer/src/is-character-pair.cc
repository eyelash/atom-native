#include "is-character-pair.h"

static bool isHighSurrogate(char16_t charCode) {
  return (0xD800 <= charCode && charCode <= 0xDBFF);
}

static bool isLowSurrogate(char16_t charCode) {
  return (0xDC00 <= charCode && charCode <= 0xDFFF);
}

static bool isVariationSelector(char16_t charCode) {
  return (0xFE00 <= charCode && charCode <= 0xFE0F);
}

static bool isCombiningCharacter(char16_t charCode) {
  return (0x0300 <= charCode && charCode <= 0x036F) || (0x1AB0 <= charCode && charCode <= 0x1AFF) || (0x1DC0 <= charCode && charCode <= 0x1DFF) || (0x20D0 <= charCode && charCode <= 0x20FF) || (0xFE20 <= charCode && charCode <= 0xFE2F);
}

static bool isCombinedCharacter(char16_t charCodeA, char16_t charCodeB) {
  return !isCombiningCharacter(charCodeA) && isCombiningCharacter(charCodeB);
}

static bool isSurrogatePair(char16_t charCodeA, char16_t charCodeB) {
  return isHighSurrogate(charCodeA) && isLowSurrogate(charCodeB);
}

static bool isVariationSequence(char16_t charCodeA, char16_t charCodeB) {
  return !isVariationSelector(charCodeA) && isVariationSelector(charCodeB);
}

bool isCharacterPair(char16_t charCodeA, char16_t charCodeB) {
  return isSurrogatePair(charCodeA, charCodeB) || isVariationSequence(charCodeA, charCodeB) || isCombinedCharacter(charCodeA, charCodeB);
}
