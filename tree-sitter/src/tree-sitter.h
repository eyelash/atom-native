#ifndef TREE_SITTER_H_
#define TREE_SITTER_H_

#include <tree_sitter/api.h>
#include <native-point.h>

unsigned startIndex(TSNode);
unsigned endIndex(TSNode);
unsigned startIndex(TSRange);
unsigned endIndex(TSRange);
NativePoint startPosition(TSNode);
NativePoint endPosition(TSNode);
NativePoint startPosition(TSRange);
NativePoint endPosition(TSRange);
std::vector<TSNode> children(TSNode);
std::vector<TSNode> descendantsOfType(TSNode, const std::vector<std::string> &, const NativePoint &, const NativePoint &);

#endif // TREE_SITTER_H_
