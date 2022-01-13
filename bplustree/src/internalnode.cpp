#include "internalnode.hpp"
#include "leafnode.hpp"
#include <cmath>

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

void InternalNode::remove(const int &key) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			children.erase(children.begin() + (low + 1 - keys.begin()));
			keys.erase(low);
		}
	}
}

void InternalNode::removeByKeyIndex(int keyIndex) {
	children.erase(children.begin() + keyIndex + 1);
	keys.erase(keys.begin() + keyIndex);
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

void InternalNode::redistribute(InternalNode *node, InternalNode *sibling, bool &siblingIsOnRHS, const int &splittingKey, int &splittingKeyIndex) {
	/*
	Redistributes keys and children between two internal nodes that are siblings.
	Assumes that this internal node is their parent.
	*/
	std::vector<int> *nodeKeys = node->getKeys();
	std::vector<int> *siblingKeys = sibling->getKeys();
	std::vector<Node *> *nodeChildren = node->getChildren();
	std::vector<Node *> *siblingChildren = sibling->getChildren();
	int totKeys = nodeKeys->size() + siblingKeys->size();
	int nodeNumKeysToReceive = std::floor(static_cast<double>(totKeys) / 2) - nodeKeys->size();
	// push from right to left through parent
	if (siblingIsOnRHS) {
		nodeKeys->insert(nodeKeys->end(), splittingKey);
		keys[splittingKeyIndex] = (*siblingKeys)[nodeNumKeysToReceive - 1];
		nodeKeys->insert(nodeKeys->end(), siblingKeys->begin(), siblingKeys->begin() + nodeNumKeysToReceive - 1);
		siblingKeys->erase(siblingKeys->begin(), siblingKeys->begin() + nodeNumKeysToReceive);
		nodeChildren->insert(nodeChildren->end(), siblingChildren->begin(), siblingChildren->begin() + nodeNumKeysToReceive);
		siblingChildren->erase(siblingChildren->begin(), siblingChildren->begin() + nodeNumKeysToReceive);
	}
	// push from left to right through parent
	else {
		nodeKeys->insert(nodeKeys->begin(), splittingKey);
		keys[splittingKeyIndex] = (*siblingKeys)[siblingKeys->size() - nodeNumKeysToReceive];
		nodeKeys->insert(nodeKeys->begin(), siblingKeys->end() - nodeNumKeysToReceive + 1, siblingKeys->end());
		siblingKeys->erase(siblingKeys->end() - nodeNumKeysToReceive, siblingKeys->end());
		nodeChildren->insert(nodeChildren->begin(), siblingChildren->end() - nodeNumKeysToReceive, siblingChildren->end());
		siblingChildren->erase(siblingChildren->end() - nodeNumKeysToReceive, siblingChildren->end());
	}
}

void InternalNode::redistribute(LeafNode *node, LeafNode *sibling, bool &siblingIsOnRHS, const int &splittingKey, int &splittingKeyIndex) {
	/*
	Redistributes keys and values between two leaf nodes that are siblings.
	Assumes that this internal node is their parent.
	*/
	std::vector<int> *nodeKeys = node->getKeys();
	std::vector<int> *siblingKeys = sibling->getKeys();
	std::vector<std::vector<int> *> *nodeValues = node->getValues();
	std::vector<std::vector<int> *> *siblingValues = sibling->getValues();
	int totKeys = nodeKeys->size() + siblingKeys->size();
	int nodeNumKeysToReceive = std::floor(static_cast<double>(totKeys) / 2) - nodeKeys->size();
	// push from right to left through parent
	if (siblingIsOnRHS) {
		keys[splittingKeyIndex] = (*siblingKeys)[nodeNumKeysToReceive];
		nodeKeys->insert(nodeKeys->end(), siblingKeys->begin(), siblingKeys->begin() + nodeNumKeysToReceive);
		siblingKeys->erase(siblingKeys->begin(), siblingKeys->begin() + nodeNumKeysToReceive);
		nodeValues->insert(nodeValues->end(), siblingValues->begin(), siblingValues->begin() + nodeNumKeysToReceive);
		siblingValues->erase(siblingValues->begin(), siblingValues->begin() + nodeNumKeysToReceive);
	}
	// push from left to right through parent
	else {
		keys[splittingKeyIndex] = (*siblingKeys)[siblingKeys->size() - nodeNumKeysToReceive];
		nodeKeys->insert(nodeKeys->begin(), siblingKeys->end() - nodeNumKeysToReceive, siblingKeys->end());
		siblingKeys->erase(siblingKeys->end() - nodeNumKeysToReceive, siblingKeys->end());
		nodeValues->insert(nodeValues->begin(), siblingValues->end() - nodeNumKeysToReceive, siblingValues->end());
		siblingValues->erase(siblingValues->end() - nodeNumKeysToReceive, siblingValues->end());
	}
}

void InternalNode::merge(InternalNode *sibling, const int &splittingKey) {
	/*
	Merge all entries of sibling (assumed sibling is on rhs) into
	this internal node, and then delete the now unneeded sibling.
	As called in remove of Bplustree clean up of invalid parent entry
	is dealt with by the respectiv parent when its recursive call returns.
	*/
	keys.push_back(splittingKey);
	std::vector<int> *siblingKeys = sibling->getKeys();
	std::vector<Node *> *siblingChildren = sibling->getChildren();
	keys.insert(keys.end(), siblingKeys->begin(), siblingKeys->end());
	children.insert(children.end(), siblingChildren->begin(), siblingChildren->end());
	delete sibling;
}
