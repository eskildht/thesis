#include "node.hpp"

bool Node::isLeaf() {
	return leaf;
}

std::vector<int> *Node::getKeys() {
	return &keys;
}

LeafNode::LeafNode() : next(this), prev(this) {
	leaf = true;
};

InternalNode::InternalNode() {
	leaf = false;
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

void LeafNode::insert(int key, int value) {
	std::vector<int> *values = getValues(key);
	if (values) {
		values->push_back(value);
	}
	else {
		std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
		keys.insert(low, key);
		values = new std::vector<int>();
		values->push_back(value);
		this->values.push_back(values);
	}
}

Node *InternalNode::split(int *keyToParent) {
	int length = keys.size();
	InternalNode *right = new InternalNode();
	*keyToParent = keys[length / 2];

	right->keys.assign(keys.begin() + length / 2 + 1, keys.end());
	keys.erase(keys.begin() + length / 2, keys.end());

	right->children.assign(children.begin() + length / 2 + 1, children.end());
	children.erase(children.begin() + length / 2 + 1, children.end());

	return right;
}

Node *LeafNode::split(int *keyToParent) {
	int length = keys.size();
	LeafNode *right = new LeafNode();
	*keyToParent = keys[length / 2];

	right->keys.assign(keys.begin() + length / 2, keys.end());
	keys.erase(keys.begin() + length / 2, keys.end());
	right->values.assign(values.begin() + length / 2, values.end());
	values.erase(values.begin() + length / 2, values.end());

	right->prev = this;
	right->next = this->next;
	this->next = right;
	(right->next)->prev = right;

	return right;
}

std::vector<Node *> *InternalNode::getChildren() {
	return &children;
}

std::vector<int> *LeafNode::getValues(int key) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			return values[low - keys.begin()];
		}
	}
	return nullptr;
}

LeafNode *LeafNode::getPrev() {
	return prev;
}

LeafNode *LeafNode::getNext() {
	return next;
}

void LeafNode::update(int key, std::vector<int> *values) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			this->values[low - keys.begin()] = values;
		}
	}
}
