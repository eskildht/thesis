#include "node.hpp"
#include <cmath>

Node::~Node() {};

std::vector<int> *Node::getKeys() {
	return &keys;
}

bool Node::isLeaf() {
	return leaf;
}

bool Node::hasExtraEntries(int &order) {
	return keys.size() > std::ceil(order / 2) - 1;
}
