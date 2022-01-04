#include <stack>
#include <string>
#include <vector>

class Node;

class Bplustree {
	public:
		Bplustree(int order);
		~Bplustree();
		int getOrder();
		void insert(int key, int value);
		void update(int key, const std::vector<int> &values);
		std::vector<int> *search(int key);
		void show();

	private:
		int order;
		Node *root;
		void findSearchPath(int key, Node* node, std::stack<Node *> *path);
		void printTree(Node *node, int level, std::string prevString, std::vector<int> *parentKeyLengths);
		std::string printNode(Node *node, int level, std::string prevString, std::vector<int> *parentKeyLengths);
		void destroy(Node *node);
};
