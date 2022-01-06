#include "node.hpp"

class InternalNode : public Node {
	public:
		InternalNode();
		std::vector<Node *> *getChildren();
		void insert(int key, Node* right);
		void insert(int key, Node* left, Node* right);
		InternalNode *split(int *keyToParent);

	private:
		std::vector<Node *> children;
};
