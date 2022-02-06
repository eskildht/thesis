#include "bplustree.hpp"
#include "thread_pool.hpp"
#include "bloom_filter.hpp"
#include <shared_mutex>

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters);
		~ParallelBplustree();
		void insert(const int key, const int value);
		void insert(std::vector<int> &keys, std::vector<int> &values);
		void update(const int key, const std::vector<int> &values);
		std::future<std::vector<std::future<const std::vector<int> *>>> search(const int key);
		std::future<std::vector<std::future<bool>>> remove(const int key);
		void show();
		void waitForWorkToFinish();
		//void readjustTreeNumInsertOp();
		std::vector<int> getTreeNumKeys();
		int getOrder();
		int getNumThreads();
		int getNumTrees();
		bool areBloomFiltersUsed();
		void pauseThreadPool();
		void resumeThreadPool();

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
		void threadInsert(const int key, const int value);
		void threadInsert(std::vector<int>::iterator keysSplitBegin, std::vector<int>::iterator keysSplitEnd, std::vector<int>::iterator valuesSplitBegin, const int treeIndex = -1);
		const std::vector<int> *threadSearch(const int key, const int treeIndex) const;
		void threadSearchCoordinator(const int key, std::promise<std::vector<std::future<const std::vector<int> *>>> *prom);
		bool threadUpdate(const int key, const std::vector<int> &values, const int treeIndex);
		void threadUpdateCoordinator(const int key, const std::vector<int> &values);
		bool threadRemove(const int key, const int treeIndex);
		void threadRemoveCoordinator(const int key, std::promise<std::vector<std::future<bool>>> *prom);
};

