#ifndef NODE_HPP
#define NODE_HPP

#include <vector>

class Node {
	public:
		virtual ~Node() = 0;
		std::vector<int> *getKeys();
		bool isLeaf();
		bool hasExtraEntries(int &order);
		virtual void remove(int &key) = 0;
		virtual Node *split(int *keyToParent) = 0;

	protected:
		std::vector<int> keys;
		bool leaf;
};

#endif 
