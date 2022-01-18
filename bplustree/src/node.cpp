#include "node.hpp"
#include <cmath>

Node::~Node() {};

std::vector<int> *Node::getKeys() {
	return &keys;
}

bool Node::isLeaf() const {
	return leaf;
}

bool Node::hasExtraEntries(const int order, const Node *root) const {
	if (!(this == root)) {
		return keys.size() > std::ceil(static_cast<double>(order) / 2) - 1;
	}
	else {
		if (leaf) {
			return true;
		}
		else {
			return keys.size() > 1;
		}
	}
}

bool Node::hasUnderflow(const int order, const Node *root) const {
	if (!(this == root)) {
		return (keys.size() < std::ceil(static_cast<double>(order) / 2) - 1);
	}
	else {
		if (leaf) {
			return false;
		}
		else {
			return keys.size() < 1;
		}
	}
}
