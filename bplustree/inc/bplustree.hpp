#include <stack>
#include <string>
#include <vector>
#include <map>

class Node;
class LeafNode;

class Bplustree {
	public:
		Bplustree(int order);
		~Bplustree();
		int getOrder();
		void insert(int key, int value);
		void update(int key, const std::vector<int> &values);
		const std::vector<int> *search(int key);
		std::map<int, std::vector<int>> scan(int start, int end);
		std::map<int, std::vector<int>> scanFull();
		void show();

	private:
		int order;
		Node *root;
		void findSearchPath(int key, Node* node, std::stack<Node *> *path);
		LeafNode *getLeftLeaf();
		void printTree(Node *node, int level, std::string prevString, std::vector<int> *parentKeyLengths);
		std::string printNode(Node *node, int level, std::string prevString, std::vector<int> *parentKeyLengths);
		void destroy(Node *node);
};
