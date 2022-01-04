#include "node.hpp"

std::vector<int> *Node::getKeys() {
	return &keys;
}

bool Node::isLeaf() {
	return leaf;
}
