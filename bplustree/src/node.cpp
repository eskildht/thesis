#include "node.hpp"
#include <cmath>

Node::~Node() {};

std::vector<int> *Node::getKeys() {
	return &keys;
}

bool Node::isLeaf() {
	return leaf;
}

bool Node::hasExtraEntries(const int &order) {
	return keys.size() > std::ceil(static_cast<double>(order) / 2) - 1;
}

bool Node::hasUnderflow(int order, Node *root) {
	return (keys.size() < std::ceil(static_cast<double>(order) / 2) - 1) && !(this == root);
}
