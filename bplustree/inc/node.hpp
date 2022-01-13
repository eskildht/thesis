#ifndef NODE_HPP
#define NODE_HPP

#include <vector>

class Node {
	public:
		virtual ~Node() = 0;
		std::vector<int> *getKeys();
		bool isLeaf();
		bool hasExtraEntries(const int &order);
		bool hasUnderflow(int order, Node *root);
		virtual void remove(const int &key) = 0;
		virtual Node *split(int *keyToParent) = 0;

	protected:
		std::vector<int> keys;
		bool leaf;
};

#endif
