#include <vector>

class Node
{
public:
	bool isLeaf();
	std::vector<int> *getKeys();
	virtual Node *split() = 0;
	virtual void insert(int key, int value) {};
	virtual void insert(int key, Node* right) {};
	virtual void insert(int key, Node* left, Node* right) {};

protected:
	bool leaf;
	std::vector<int> keys;
};

class InternalNode : public Node
{
public:
	InternalNode();
	Node *split();
	void insert(int key, Node* right);
	void insert(int key, Node* left, Node* right);
	std::vector<Node *> *getChildren();

private:
	std::vector<Node *> children;
};

class LeafNode : public Node
{
public:
	LeafNode();
	Node *split();
	void insert(int key, int value);
	std::vector<int> *getValues(int key);
	LeafNode *getPrev();
	LeafNode *getNext();

private:
	std::vector<std::vector<int> *> values;
	LeafNode *next;
	LeafNode *prev;
};
