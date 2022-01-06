#include "internalnode.hpp"

InternalNode::InternalNode() {
	leaf = false;
}

std::vector<Node *> *InternalNode::getChildren() {
	return &children;
};

void InternalNode::insert(int key, Node *right) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	keys.insert(low, key);
	low = std::lower_bound(keys.begin(), keys.end(), key);
	children.insert(children.begin() + (low - keys.begin() + 1), right);
}

void InternalNode::insert(int key, Node *left, Node *right) {
	keys.push_back(key);
	children.push_back(left);
	children.push_back(right);
}

InternalNode *InternalNode::split(int *keyToParent) {
	int length = keys.size();
	InternalNode *right = new InternalNode();
	*keyToParent = keys[length / 2];

	right->keys.assign(keys.begin() + length / 2 + 1, keys.end());
	keys.erase(keys.begin() + length / 2, keys.end());

	right->children.assign(children.begin() + length / 2 + 1, children.end());
	children.erase(children.begin() + length / 2 + 1, children.end());

	return right;
}
