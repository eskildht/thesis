#ifndef NODE_HPP
#define NODE_HPP
#include "node.hpp"
#endif 

class InternalNode : public Node {
	public:
		InternalNode();
		std::vector<Node *> *getChildren();
		void insert(int key, Node* right);
		void insert(int key, Node* left, Node* right);
		Node *split(int *keyToParent);

	private:
		std::vector<Node *> children;
};
