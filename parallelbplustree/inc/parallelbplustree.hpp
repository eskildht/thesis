#include "bplustree.hpp"
#include "ctpl_stl.hpp"
#include <vector>

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads);
		void insert(const int key, const int value);
		void show();

	private:
		int order;
		int numThreads;
		ctpl::thread_pool threadPool;
		std::vector<Bplustree *> trees;
};
