#include "node.hpp"
#include <tuple>

class LeafNode;

class InternalNode : public Node {
	public:
		InternalNode();
		std::vector<Node *> *getChildren();
		void insert(int key, Node* right);
		void insert(int key, Node* left, Node* right);
		void remove(const int &key);
		void removeByKeyIndex(int keyIndex);
		InternalNode *split(int *keyToParent);
		std::tuple<Node *, bool, const int &, int> getSibling(Node *node, int &order, Node *root);
		void redistribute(InternalNode *node, InternalNode *sibling, bool &siblingIsOnRHS, const int &splittingKey, int &splittingKeyIndex);
		void redistribute(LeafNode *node, LeafNode *sibling, bool &siblingIsOnRHS, const int &splittingKey, int &splittingKeyIndex);
		void merge(InternalNode *sibling, const int &splittingKey);

	private:
		std::vector<Node *> children;
};
