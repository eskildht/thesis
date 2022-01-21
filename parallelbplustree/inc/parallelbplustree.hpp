#include "bplustree.hpp"
#include "ctpl_stl.hpp"
#include <future>
#include <vector>

class ParallelBplustree {
	public:
		ParallelBplustree(const int order, const int numThreads);
		~ParallelBplustree();
		std::future<void> insert(const int key, const int value);
		void show();

	private:
		int order;
		int numThreads;
		std::vector<std::mutex *> locks;
		ctpl::thread_pool threadPool;
		std::vector<Bplustree *> trees;
};
