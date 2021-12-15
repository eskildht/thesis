#include "node.hpp"
#include <string>

bool Node::isLeaf() {
	return leaf;
}

std::vector<int>& Node::getKeys() {
	return keys;
}

std::vector<Node>& InternalNode::getChildren() {
	return children;
}

std::vector<int>& LeafNode::getValues(int key) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (key == keys[low - keys.begin()]) {
		return values[low - keys.begin()];
	}
	std::string msg = "Key=" + std::to_string(key) + " not found in leaf node";
	throw std::invalid_argument(msg);
}
