#include "grammar.h"

Grammar::Grammar(const char *name, const char *scopeName) {
  this->name = name;
  this->scopeName = scopeName;
}

Grammar::~Grammar() {}

void Grammar::addFileTypes(const char *fileType) {
  this->fileTypes.push_back(fileType);
}
