#include "bplustree.hpp"
#include "ctpl_stl.hpp"
#include "bloom_filter.hpp"
#include <vector>

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilter);
		~ParallelBplustree();
		void insert(const int key, const int value);
		void show();

	private:
		const int order;
		const int numThreads;
		const int numTrees;
		const bool useBloomFilter;
		std::vector<Bplustree *> trees;
		std::vector<std::mutex *> treeLocks;
		std::vector<bloom_filter *> filters;
		ctpl::thread_pool threadPool;
};
