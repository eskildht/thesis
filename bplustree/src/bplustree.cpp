#include "bplustree.hpp"
#include "internalnode.hpp"
#include "leafnode.hpp"
#include <cmath>
#include <iostream>

Bplustree::Bplustree(int order) : order(order) {
	root = new LeafNode();
}

int Bplustree::getOrder() {
	return order;
}

void Bplustree::insert(int key, int value) {
	std::stack<Node *> path;
	findSearchPath(key, root, &path);
	LeafNode *leaf = static_cast<LeafNode *>(path.top());
	path.pop();
	leaf->insert(key, value);
if (leaf->getKeys()->size() == order) {
	int *keyToParent = new int;
	Node *right = leaf->split(keyToParent);
	if (path.empty()) {
		InternalNode *newRoot = new InternalNode();
		newRoot->insert(*keyToParent, leaf, right);
		root = newRoot;
		return;
	}
	InternalNode *internal = static_cast<InternalNode *>(path.top());
	path.pop();
	internal->insert(*keyToParent, right);
	while (internal->getKeys()->size() == order) {
		right = internal->split(keyToParent);
		if (!path.empty()) {
			internal = static_cast<InternalNode *>(path.top());
			path.pop();
			internal->insert(*keyToParent, right);
		}
		else {
			InternalNode *newRoot = new InternalNode();
			newRoot->insert(*keyToParent, internal, right);
			root = newRoot;
			return;
		}
	}
	delete keyToParent;
}
else {
	return;
}
}

void Bplustree::update(int key, const std::vector<int> &values) {
	std::stack<Node *> path;
	findSearchPath(key, root, &path);
	LeafNode *leaf = static_cast<LeafNode *>(path.top());
	leaf->update(key, values);
}

void Bplustree::findSearchPath(int key, Node *node, std::stack<Node *> *path) {
	path->push(node);
	if (node->isLeaf()) {
		return;
	}
	std::vector<int> *keys = node->getKeys();
	std::vector<int>::iterator low = std::lower_bound(keys->begin(), keys->end(), key);
	InternalNode *internal = static_cast<InternalNode *>(node);
	Node *nextNode = nullptr;
	if (key == (*keys)[low - keys->begin()]) {
		nextNode = (*(internal->getChildren()))[low - keys->begin() + 1];
	}
	else {
		nextNode = (*(internal->getChildren()))[low - keys->begin()];
	}
	findSearchPath(key, nextNode, path);
}

const std::vector<int> *Bplustree::search(int key) {
	std::stack<Node *> path;
	findSearchPath(key, root, &path);
	LeafNode *leaf = static_cast<LeafNode *>(path.top());
	return leaf->getValues(key);
}

std::map<int, std::vector<int>> Bplustree::scan(int start, int end) {
	std::stack<Node *> path;
	findSearchPath(start, root, &path);
	LeafNode *startLeaf = static_cast<LeafNode *>(path.top());
	std::map<int, std::vector<int>> result;
	LeafNode *leaf = startLeaf->scan(start, end, startLeaf, result);
	while (leaf) {
		leaf = leaf->scan(start, end, startLeaf, result);
	}
	return result;
}

std::map<int, std::vector<int>> Bplustree::scanFull() {
	LeafNode *startLeaf = getLeftLeaf();
	std::map<int, std::vector<int>> result;
	LeafNode *leaf = startLeaf->scanFull(result);
	while (startLeaf != leaf) {
		leaf = leaf->scanFull(result);
	}
	return result;
}

LeafNode *Bplustree::getLeftLeaf() {
	Node *node = root;
	while (!node->isLeaf()) {
		InternalNode *internal = static_cast<InternalNode *>(node);
		node = (*(internal->getChildren()))[0];
	}
	return static_cast<LeafNode *>(node);
}

void Bplustree::remove(int key) {
	/*
	Database Management Systems, 3rd Edition pp. 352-356
 	used as guideline.
	*/
	remove(nullptr, root, key, nullptr);
}

void Bplustree::remove(InternalNode *parent, Node *node, int &key, Node *oldChildEntry) {
	if (!node->isLeaf()) {
		InternalNode *internal = static_cast<InternalNode *>(node);
		std::vector<int> *keys = internal->getKeys();
		std::vector<int>::iterator low = std::lower_bound(keys->begin(), keys->end(), key);
		Node *nextNode = nullptr;
		if (key == (*keys)[low - keys->begin()]) {
			nextNode = (*(internal->getChildren()))[low - keys->begin() + 1];
		}
		else {
			nextNode = (*(internal->getChildren()))[low - keys->begin()];
		}
		remove(node, nextNode, key, oldChildEntry);
		if (!oldChildEntry) {
			return;
		}
		else {
			internal->remove(oldChildEntry);
			if (!internal->hasExtraEntries(order)) {
				oldChildEntry = nullptr;
				return;
			}
			else {
				auto [sibling, siblingIsOnRHS, splittingKey] = parent->getSibling(internal);
				if (sibling->hasExtraEntries(order)) {
					parent->redistribute(internal, sibling, siblingIsOnRHS);
					oldChildEntry = nullptr;
					return;
				}
				else {
					if (siblingIsOnRHS) {
						oldChildEntry = sibling;
						internal->insert(splittingKey, (*(sibling->getChildren()))[0]);
						internal->merge(sibling);
						return;
					}
					else {
						oldChildEntry = internal;
						sibling->insert(splittingKey, (*(internal->getChildren()))[0]);
						sibling->merge(internal);
						return;
					}
				}
			}
		}
	}
	else {
		LeafNode *leaf = static_cast<LeafNode *>(node);
		if (leaf->hasExtraEntries(order)) {
			leaf->remove(key);
			oldChildEntry = nullptr;
			return;
		}
		else {
			auto [sibling, siblingIsOnRHS, splittingKeyIndex] = parent->getSibling(leaf);
			if (sibling->hasExtraEntries(order)) {
				parent->redistribute(leaf, sibling, siblingIsOnRHS);
				(*(parent->getKeys()))[splittingKeyIndex] = siblingIsOnRHS ? (*(sibling->getKeys()))[0] : (*(leaf->getKeys()))[0];
				oldChildEntry = nullptr;
				return;
			}
			else {
				if (siblingIsOnRHS) {
					oldChildEntry = sibling;
					leaf->merge(sibling);
					return;
				}
				else {
					oldChildEntry = leaf;
					sibling->merge(leaf);
					return;
				}
			}
		}
	}
}

void Bplustree::show() {
	/*
	Method that prints the current tree.
	Inspired by: github.com/EmilianoCarrillo/BTree-Pretty-Print
	*/
	std::vector<int> parentKeyLengths;
	printTree(root, 0, "", &parentKeyLengths);
}

void Bplustree::printTree(Node *node, int printLevel, std::string prevString, std::vector<int> *parentKeyLengths) {
	std::string baseString = prevString;
	prevString = printNode(node, printLevel, baseString, parentKeyLengths);
	if (!node->isLeaf()) {
		std::vector<int> parentKeyLengthsToPass;
		for (int i = 0; i < node->getKeys()->size(); i++) {
			parentKeyLengthsToPass.push_back(std::to_string((*(node->getKeys()))[i]).length());
		}
		InternalNode *internal = static_cast<InternalNode *>(node);
		for (int i = internal->getChildren()->size() - 1; i >= 0; i--) {
			printTree((*(internal->getChildren()))[i], i, prevString, &parentKeyLengthsToPass);
		}
	}
}

std::string Bplustree::printNode(Node *node, int printLevel, std::string prevString, std::vector<int> *parentKeyLengths) {
	prevString += parentKeyLengths->empty() ? "" : "     ";
	for (int i = 1; i <= printLevel; i++) {
		prevString += "|";
		for (int j = 0; j < (*parentKeyLengths)[i-1]; j++) {
			prevString += " ";
		}
	}

	std::cout << prevString << " `---|";
	for (int i = 0; i < node->getKeys()->size(); i++) {
		std::cout << (*(node->getKeys()))[i] << "|";
	}
	std::cout << std::endl;
	return prevString;
}

Bplustree::~Bplustree() {
	destroy(root);
}

void Bplustree::destroy(Node *node) {
	if(!node->isLeaf()) {
		InternalNode *internal = static_cast<InternalNode *>(node);
		for (Node *child : *(internal->getChildren())) {
			destroy(child);
		}
	}
	delete node;
}
