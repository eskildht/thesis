#include "node.hpp"
#include <map>

class LeafNode : public Node {
	public:
		LeafNode();
		~LeafNode();
		LeafNode *getNext();
		LeafNode *getPrev();
		std::vector<int> *getValues(int key);
		std::vector<std::vector<int> *> *getValues();
		LeafNode *scan(int start, int end, LeafNode *startLeaf, std::map<int, std::vector<int>> &result);
		LeafNode *scanFull(std::map<int, std::vector<int>> &result);
		void insert(int key, int value);
		LeafNode *split(int *keyToParent);
		void update(int key, const std::vector<int> &values);
		void remove(int &key);

	private:
		LeafNode *next;
		LeafNode *prev;
		std::vector<std::vector<int> *> values;
};
