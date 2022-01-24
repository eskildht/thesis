#include <stack>
#include <string>
#include <vector>
#include <map>

class Node;
class InternalNode;
class LeafNode;

class Bplustree {
	public:
		Bplustree(const int order);
		~Bplustree();
		int getOrder();
		void insert(const int key, const int value);
		bool update(const int key, const std::vector<int> &values);
		const std::vector<int> *search(const int key);
		std::map<int, std::vector<int>> scan(const int start, const int end);
		std::map<int, std::vector<int>> scanFull();
		void remove(const int key);
		void show();

	private:
		int order;
		Node *root;
		void findSearchPath(const int key, Node* node, std::stack<Node *> *path);
		void remove(InternalNode *parent, Node *node, const int key, int *&oldChildEntry);
		LeafNode *getLeftLeaf();
		void printTree(Node *node, const int level, std::string prevString, const std::vector<int> *parentKeyLengths);
		std::string printNode(Node *node, const int level, std::string prevString, const std::vector<int> *parentKeyLengths);
		void destroy(Node *node);
};
