#include "node.hpp"
#include <string>

bool Node::isLeaf()
{
	return leaf;
}

std::vector<int> *Node::getKeys()
{
	return &keys;
}

LeafNode::LeafNode() : next(this), prev(this)
{
	leaf = true;
};

InternalNode::InternalNode()
{
	leaf = false;
};

Node *InternalNode::split()
{
	int length = keys.size();
	InternalNode *right = new InternalNode();

	right->keys.assign(keys.begin() + length / 2 + 1, keys.end());
	keys.erase(keys.begin() + length / 2 + 1, keys.end())

		right->children.assign(keys.begin() + length / 2 + 1, keys.end());
	children.erase(keys.begin() + length / 2 + 1, keys.end());

	return right;
}

Node *LeafNode::split()
{
	int length = keys.size();
	LeafNode *right = new LeafNode();

	right->keys.assign(keys.begin() + length / 2 + 1, keys.end());
	keys.erase(keys.begin() + length / 2 + 1, keys.end())
		right->values.assign(values.begin() + length / 2 + 1, values.end());
	keys.erase(values.begin() + length / 2 + 1, values.end())

		right->prev = this;
	right->next = this->next;
	this->next = right;
	(right->next)->prev = right;

	return right;
}

std::vector<Node> &InternalNode::getChildren()
{
	return &children;
}

std::vector<int> &LeafNode::getValues(int key)
{
	std::vector<int>::iterator low = std::lower_bound(keys.begin(), keys.end(), key);
	if (key == keys[low - keys.begin()])
	{
		return values[low - keys.begin()];
	}
	std::string msg = "Key=" + std::to_string(key) + " not found in leaf node";
	throw std::invalid_argument(msg);
}
