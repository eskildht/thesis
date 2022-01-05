#include "node.hpp"

Node::~Node() {};

std::vector<int> *Node::getKeys() {
	return &keys;
}

bool Node::isLeaf() {
	return leaf;
}
