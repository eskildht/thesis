#include <vector>

class Node
{
public:
	bool isLeaf();
	std::vector<int> *getKeys();
	virtual Node *split() = 0;

protected:
	bool leaf;
	std::vector<int> keys;
};

class InternalNode : public Node
{
public:
	InternalNode();
	Node *split();
	std::vector<Node *> *getChildren();
	void insert(int key);

private:
	std::vector<Node *> children;
};

class LeafNode : public Node
{
public:
	LeafNode();
	Node *split();
	std::vector<int> *getValues(int key);
	LeafNode *getPrev();
	LeafNode *getNext();
	void insert(int key, int value);

private:
	std::vector<std::vector<int> *> values;
	LeafNode *next;
	LeafNode *prev;
};
