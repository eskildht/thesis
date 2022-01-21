#include "parallelbplustree.hpp"
#include <future>
#include <iostream>
#include <sstream>

ParallelBplustree::ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilter) : order(order), numThreads(numThreads), threadPool(numThreads), numTrees(numTrees), useBloomFilter(useBloomFilter) {
	for (int i = 0; i < numTrees; i++) {
		trees.push_back(new Bplustree(order));
		treeLocks.push_back(new std::mutex);
	}
	if (useBloomFilter) {
		bloom_parameters parameters;
		// Maximum false_positive_probability will become 1 in 1000000
		// for each filter.
		parameters.projected_element_count = 1000000; 
		parameters.compute_optimal_parameters();
		filters.push_back(new bloom_filter(parameters));
	}
}

std::future<void> ParallelBplustree::insert(const int key, const int value) {
	// Find the correct tree to insert into
	//t->insert(key, value);

	// Push the insert task into the thread pool using lambda expression
	std::future<void> fut = threadPool.push([this](int id, const int key, const int value) { 
		std::stringstream ss;
		ss << id << " ready to insert key=" << key << " value=" << value << "\n";
		std::cout << ss.str();
		this->trees[0]->insert(key, value);
	}, key, value);
	return fut;
}

void ParallelBplustree::show() {
	for (int i = 0; i < numThreads; i++) {
		trees[i]->show();
	}
}

ParallelBplustree::~ParallelBplustree() {
	threadPool.stop(true);
	for (Bplustree *tree : trees) {
		delete tree;
	}

}
