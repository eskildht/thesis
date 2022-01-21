#include "leafnode.hpp"
#include <iostream>

LeafNode::LeafNode() : next(this), prev(this) {
	leaf = true;
};

LeafNode::~LeafNode() {
	for (std::vector<int> *value : values) {
		delete value;
	}
}

LeafNode *LeafNode::getNext() const {
	return next;
}

LeafNode *LeafNode::getPrev() const {
	return prev;
}

std::vector<int> *LeafNode::getValues(const int key) {
	std::cout << "Size of keys: " << keys.size() << "\n";
	std::cout << "Size of values: " << values.size() << "\n";
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			return values[low - keys.begin()];
		}
	}
	return nullptr;
}

bool LeafNode::remove(const int key) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			values.erase(values.begin() + (low - keys.begin()));
			keys.erase(low);
			return true;
		}
	}
	return false;
}

void LeafNode::insert(const int key, const int value) {
	std::vector<int> *values = getValues(key);
	if (values) {
		values->push_back(value);
	}
	else {
		std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
		int index = low - keys.begin();
		keys.insert(low, key);
		values = new std::vector<int>();
		values->push_back(value);
		this->values.insert(this->values.begin() + index, values);
	}
}

LeafNode *LeafNode::split(int *keyToParent) {
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

LeafNode *LeafNode::scan(const int start, const int end, const LeafNode *startLeaf, std::map<int, std::vector<int>> &result) const {
	int index = 0;
	if (this == startLeaf) {
		std::vector<int>::const_iterator low = std::lower_bound(keys.begin(), keys.end(), start);
		index = low - keys.begin();
	}
	if (index < keys.size()) {
		for (int i = index; i < keys.size(); i++) {
			if (keys[i] <= end) {
				result[keys[i]] = *values[i];
			}
			else {
				return nullptr;
			}
		}
	}
	return next != startLeaf ? next : nullptr;
}

LeafNode *LeafNode::scanFull(std::map<int, std::vector<int>> &result) const {
	for (std::vector<int>::const_iterator it = keys.begin(); it != keys.end(); it++) {
		result[*it] = *values[it - keys.begin()];
	}
	return next;
}

void LeafNode::update(const int key, const std::vector<int> &values) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			this->values[low - keys.begin()] = new std::vector<int>(values);
		}
	}
}

std::vector<std::vector<int> *> *LeafNode::getValues() {
	return &values;
}

void LeafNode::merge(LeafNode *sibling) {
	/*
	Merge all entries of sibling (assumed sibling is on rhs) into
	this leaf, and then delete the now unneeded sibling. As called
	in remove of Bplustree clean up of invalid parent entry is dealt
	with by the respectiv parent when its recursive call returns.
	*/
	std::vector<int> *siblingKeys = sibling->getKeys();
	std::vector<std::vector<int> *> *siblingValues = sibling->getValues();
	keys.insert(keys.end(), siblingKeys->begin(), siblingKeys->end());
	values.insert(values.end(), siblingValues->begin(), siblingValues->end());
	siblingValues->clear();
	this->next = sibling->next;
	(sibling->next)->prev = this;
	delete sibling;
}
