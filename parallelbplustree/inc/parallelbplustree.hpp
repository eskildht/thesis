#include "bplustree.hpp"
#include "thread_pool.hpp"
#include "bloom_filter.hpp"
#include <shared_mutex>

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters);
		~ParallelBplustree();
		void insert(const int key, const int value);
		std::vector<std::future<const std::vector<int> *>> search(const int key);
		//std::vector<std::future<bool>> update(const int key, const std::vector<int> &values);
		//std::vector<std::future<bool>> updateOrInsert(const int key, const std::vector<int> &values);
		//std::vector<std::future<bool>> remove(const int key);
		void show();
		void waitForWorkToFinish();
		//void readjustTreeNumInsertOp();
		std::vector<int> getTreeNumKeys();
		int getOrder();
		int getNumThreads();
		int getNumTrees();
		bool areBloomFiltersUsed();

	private:
		const int order;
		const int numThreads;
		const int numTrees;
		const bool useBloomFilters;
		int updateOrInsertTreeSelector = 0;
		std::vector<Bplustree *> trees;
		std::vector<std::shared_mutex *> treeLocks;
		std::vector<bloom_filter *> treeFilters;
		std::vector<std::shared_mutex *> treeFilterLocks;
		thread_pool threadPool;
		AccessKey *accessKey;
		void threadInsert(const int key, const int value);
		//void threadInsert(const int key, const std::vector<int> &values, const int treeIndex);
		//const std::vector<int> *threadSearch(const int key, const int treeIndex) const;
		//std::vector<int> *threadSearch(const int key, const int treeIndex, AccessKey *accessKey);
		//bool threadUpdate(const int key, const std::vector<int> &values, const int treeIndex);
		//bool threadUpdateOrInsert(const int key, const std::vector<int> &values, const int treeIndex);
		//bool threadRemove(const int key, const int treeIndex);
};

