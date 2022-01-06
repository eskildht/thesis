#include "node.hpp"
#include <map>

class LeafNode : public Node {
	public:
		LeafNode();
		~LeafNode();
		LeafNode *getNext();
		LeafNode *getPrev();
		std::vector<int> *getValues(int key);
		LeafNode *scan(int start, int end, bool startLeaf, std::map<int, std::vector<int>> &result);
		void insert(int key, int value);
		LeafNode *split(int *keyToParent);
		void update(int key, const std::vector<int> &values);

	private:
		LeafNode *next;
		LeafNode *prev;
		std::vector<std::vector<int> *> values;
};
