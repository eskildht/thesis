#include "node.hpp"
#include <cmath>

Node::~Node() {};

std::vector<int> *Node::getKeys() {
	return &keys;
}

bool Node::isLeaf() {
	return leaf;
}

bool Node::hasExtraEntries(const int &order, Node *root) {
	if (!(this == root)) {
		return keys.size() > std::ceil(static_cast<double>(order) / 2) - 1;
	}
	else {
		if (leaf) {
			return keys.size() > 0;
		}
		else {
			return keys.size() > 2;
		}
	}
}

bool Node::hasUnderflow(int order, Node *root) {
	if (!(this == root)) {
		return (keys.size() < std::ceil(static_cast<double>(order) / 2) - 1);
	}
	else {
		if (leaf) {
			return false;
		}
		else {
			return keys.size() < 2;
		}
	}
}
