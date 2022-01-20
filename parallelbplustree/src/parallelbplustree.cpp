#include "parallelbplustree.hpp"

ParallelBplustree::ParallelBplustree(const int order, const int numThreads) : order(order), numThreads(numThreads), threadPool(numThreads) {
	for (int i = 0; i < numThreads; i++) {
		trees.push_back(new Bplustree(order));
	}
}

void ParallelBplustree::insert(const int key, const int value) {
	// Find the correct tree to insert into
	Bplustree *tree = trees[0];

	// Push the insert task into the thread pool using lambda expression
	threadPool.push([&tree](int id, const int key, const int value) { tree->insert(key, value); }, key, value);
}

void ParallelBplustree::show() {
	for (int i = 0; i < numThreads; i++) {
		trees[i]->show();
	}
}
