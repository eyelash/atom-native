#ifndef TREE_SITTER_H_
#define TREE_SITTER_H_

#include <tree_sitter/api.h>
#include <native-point.h>

struct NativeTextBuffer;

unsigned startIndex(TSNode);
unsigned endIndex(TSNode);
unsigned startIndex(TSRange);
unsigned endIndex(TSRange);
unsigned startIndex(TSTreeCursor *);
unsigned endIndex(TSTreeCursor *);
NativePoint startPosition(TSNode);
NativePoint endPosition(TSNode);
NativePoint startPosition(TSRange);
NativePoint endPosition(TSRange);
NativePoint startPosition(TSTreeCursor *);
NativePoint endPosition(TSTreeCursor *);
std::vector<TSNode> children(TSNode);
std::vector<TSNode> descendantsOfType(TSNode, const std::vector<std::string> &, const NativePoint &, const NativePoint &);
TSTree *parseTextBufferSync(TSParser *, NativeTextBuffer *, TSTree *, const std::vector<TSRange> &);

#endif // TREE_SITTER_H_