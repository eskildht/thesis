#include <vector>

class Node {
	public:
		bool isLeaf();
		std::vector<int>* getKeys();
		virtual Node* split();
	protected:
		bool leaf;
		std::vector<int> keys;
};

class InternalNode: Node {
	public:
		InternalNode();
		Node* split();
		std::vector<Node>* getChildren();
	private:
		std::vector<Node> children;
};

class LeafNode: Node {
	public:
		LeafNode();
		Node* split();
		std::vector<int>* getValues(int key);
		LeafNode* getPrev();
		LeafNode* getNext();
	private:
		std::vector<std::vector<int>> values;
		LeafNode* next;
		LeafNode* prev;
};
