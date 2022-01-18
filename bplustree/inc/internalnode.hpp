#include "node.hpp"
#include <tuple>

class LeafNode;

class InternalNode : public Node {
	public:
		InternalNode();
		std::vector<Node *> *getChildren();
		void insert(const int key, Node* right);
		void insert(const int key, Node* left, Node* right);
		bool remove(const int key);
		void removeByKeyIndex(const int keyIndex);
		InternalNode *split(int *keyToParent);
		std::tuple<Node *, bool, int, int> getSibling(const Node *node, const int order, const Node *root) const;
		void redistribute(InternalNode *node, InternalNode *sibling, const bool siblingIsOnRHS, const int splittingKey, const int splittingKeyIndex);
		void redistribute(LeafNode *node, LeafNode *sibling, const bool siblingIsOnRHS, const int splittingKey, const int splittingKeyIndex);
		void merge(InternalNode *sibling, const int splittingKey);

	private:
		std::vector<Node *> children;
};
