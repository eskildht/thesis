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

void InternalNode::remove(int &key) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			children.erase(children.begin() + (low + 1 - keys.begin()));
			keys.erase(low);
		}
	}
}

std::tuple<Node *, bool, const int &, int> InternalNode::getSibling(Node *node, int &order) {
	/*
	Returns a sibling that should be used for redistribution or merge during remove.
	If possible a sibling that allows for redistribution is returned.

	:return: sibling, siblingIsOnRHS, splittingKey, splittingKeyIndex
	*/
	std::vector<Node *>::iterator it;
	it = std::find(children.begin(), children.end(), node);
	if (it != children.end()) {
		// node found in children vector
		int index = it - children.begin();
		if ((index - 1 >= 0) && (index + 1 <= children.size() - 1)) {
			// right and left sibling exist
			if (children[index - 1]->hasExtraEntries(order)) {
				// redistribution using left is possible
				return std::make_tuple(children[index - 1], false, keys[index - 1], index - 1);
			}
			else {
				// redistribute or merge using right
				return std::make_tuple(children[index + 1], true, keys[index], index);
			}
		}
		else if (index + 1 <= children.size() - 1) {
			// only right sibling exist
			return std::make_tuple(children[index + 1], true, keys[index], index);
		}
		else {
			// only left sibling exist
			return std::make_tuple(children[index - 1], false, keys[index - 1], index - 1);
		}
	}
	throw "node not found in children";
}
