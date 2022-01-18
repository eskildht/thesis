#include "node.hpp"
#include <map>

class LeafNode : public Node {
	public:
		LeafNode();
		~LeafNode();
		LeafNode *getNext() const;
		LeafNode *getPrev() const;
		std::vector<int> *getValues(const int key) const;
		std::vector<std::vector<int> *> *getValues();
		LeafNode *scan(const int start, const int end, const LeafNode *startLeaf, std::map<int, std::vector<int>> &result) const;
		LeafNode *scanFull(std::map<int, std::vector<int>> &result) const;
		void insert(const int key, const int value);
		LeafNode *split(int *keyToParent);
		void update(const int key, const std::vector<int> &values);
		bool remove(const int key);
		void merge(LeafNode *sibling);

	private:
		LeafNode *next;
		LeafNode *prev;
		std::vector<std::vector<int> *> values;
};
