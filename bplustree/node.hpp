#include <vector>

class Node {
	public:
		bool isLeaf();
		std::vector<int>& getKeys();
	protected:
		bool leaf;
		std::vector<int> keys;
};

class InternalNode: Node {
	public:
		std::vector<Node>& getChildren();
	private:
		std::vector<Node> children;
};

class LeafNode: Node {
	public:
		std::vector<int>& getValues(int key);
	private:
		std::vector<std::vector<int>> values;
};
