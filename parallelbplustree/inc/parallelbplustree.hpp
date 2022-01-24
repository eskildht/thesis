#include "bplustree.hpp"
#include "ctpl_stl.hpp"
#include "bloom_filter.hpp"

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters);
		~ParallelBplustree();
		std::future<void> insert(const int key, const int value);
		std::vector<std::future<const std::vector<int> *>> search(const int key);
		std::vector<std::future<void>> update(const int key, const std::vector<int> &values);
		void show();
		void waitForWorkToFinish();

	private:
		const int order;
		const int numThreads;
		const int numTrees;
		const bool useBloomFilters;
		std::vector<Bplustree *> trees;
		std::vector<std::mutex *> treeLocks;
		std::vector<bloom_filter *> treeFilters;
		std::vector<int> treeNumKeys;
		ctpl::thread_pool threadPool;
		void threadInsert(const int key, const int value, const int treeIndex);
		const std::vector<int> *threadSearch(const int key, const int treeIndex) const;
};
