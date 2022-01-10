#include "node.hpp"
#include <tuple>

class InternalNode : public Node {
	public:
		InternalNode();
		std::vector<Node *> *getChildren();
		void insert(int key, Node* right);
		void insert(int key, Node* left, Node* right);
		void remove(int &key);
		InternalNode *split(int *keyToParent);
		void redistribute(InternalNode *node, InternalNode *sibling, bool &siblingIsOnRHS, const int &splittingKey, int &splittingKeyIndex);
		std::tuple<Node *, bool, const int &, int> getSibling(Node *node, int &order);

	private:
		std::vector<Node *> children;
};
