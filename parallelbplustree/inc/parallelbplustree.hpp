#include "bplustree.hpp"
#include "ctpl_stl.hpp"
#include "bloom_filter.hpp"

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters);
		~ParallelBplustree();
		std::future<void> insert(const int key, const int value);
		std::vector<std::future<const std::vector<int> *>> search(const int key);
		std::vector<std::future<void>> updateOrInsert(const int key, const std::vector<int> &values);
		std::vector<std::future<bool>> remove(const int key);
		void show();
		void waitForWorkToFinish();
		void readjustTreeNumUniqueKeyInsertOp();
		const std::vector<int> &getTreeNumKeys();

	private:
		const int order;
		const int numThreads;
		const int numTrees;
		const bool useBloomFilters;
		std::vector<Bplustree *> trees;
		std::vector<std::mutex *> treeLocks;
		std::vector<bloom_filter *> treeFilters;
		std::vector<int> treeNumKeys;
		std::vector<int> treeNumUniqueKeyInsertOp;
		std::vector<std::mutex *> treeNumKeysLocks;
		ctpl::thread_pool threadPool;
		void threadInsert(const int key, const int value, const int treeIndex);
		const std::vector<int> *threadSearch(const int key, const int treeIndex) const;
		void threadUpdateOrInsert(const int key, const std::vector<int> &values, const int treeIndex);
		bool threadRemove(const int key, const int treeIndex);
};
