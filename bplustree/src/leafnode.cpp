#include "leafnode.hpp"
#include <cstddef>

LeafNode::LeafNode() : next(this), prev(this) {
	leaf = true;
};

LeafNode::~LeafNode() {
	for (std::vector<int> *value : values) {
		delete value;
	}
}

LeafNode *LeafNode::getNext() {
	return next;
}

LeafNode *LeafNode::getPrev() {
	return prev;
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

LeafNode *LeafNode::scan(int start, int end, bool startLeaf, std::map<int, std::vector<int>> &result) {
	int index = 0;
	if (startLeaf) {
		std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), start);
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
		return next != this ? next : nullptr;
	}
	return nullptr;
}

void LeafNode::update(int key, const std::vector<int> &values) {
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (low - keys.begin() < keys.size()) {
		if (key == keys[low - keys.begin()]) {
			this->values[low - keys.begin()] = new std::vector<int>(values);
		}
	}
}
