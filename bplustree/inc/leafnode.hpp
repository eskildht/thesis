#ifndef NODE_HPP
#define NODE_HPP
#include "node.hpp"
#endif 

class LeafNode : public Node {
	public:
		LeafNode();
		~LeafNode();
		LeafNode *getNext();
		LeafNode *getPrev();
		std::vector<int> *getValues(int key);
		void insert(int key, int value);
		Node *split(int *keyToParent);
		void update(int key, const std::vector<int> &values);

	private:
		LeafNode *next;
		LeafNode *prev;
		std::vector<std::vector<int> *> values;
};
