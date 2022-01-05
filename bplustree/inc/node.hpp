#include <vector>

class Node {
	public:
		virtual ~Node() = 0;
		std::vector<int> *getKeys();
		virtual void insert(int key, int value) {};
		virtual void insert(int key, Node* right) {};
		virtual void insert(int key, Node* left, Node* right) {};
		bool isLeaf();
		virtual Node *split(int *keyToParent) = 0;

	protected:
		std::vector<int> keys;
		bool leaf;
};
