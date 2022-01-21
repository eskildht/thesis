#include "bplustree.hpp"
#include "ctpl_stl.hpp"
#include "bloom_filter.hpp"

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters);
		~ParallelBplustree();
		void insert(const int key, const int value);
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
		void threadInsert(int id, const int key, const int value, const int treeIndex);
};
