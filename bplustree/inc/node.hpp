#ifndef NODE_HPP
#define NODE_HPP

#include <vector>

class Node {
	public:
		virtual ~Node() = 0;
		std::vector<int> *getKeys();
		bool isLeaf() const;
		bool hasExtraEntries(const int order, const Node *root) const;
		bool hasUnderflow(const int order, const Node *root) const;
		virtual bool remove(const int key) = 0;
		virtual Node *split(int *keyToParent) = 0;

	protected:
		std::vector<int> keys;
		bool leaf;
};

#endif
