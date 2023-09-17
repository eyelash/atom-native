#ifndef PARSER_H_
#define PARSER_H_

#include "tree.h"

struct TSParser;
struct TSLanguage;
class NativeTextBuffer;

class Parser {
  TSParser *parser;

public:
  Parser();
  ~Parser();

  void setLanguage(const TSLanguage *);
  Tree parseTextBufferSync(NativeTextBuffer *, const Tree &, const std::vector<TSRange> &);
};

#endif // PARSER_H_
