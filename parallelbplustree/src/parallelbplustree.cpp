#include "parallelbplustree.hpp"
#include <future>
#include <iostream>
#include <sstream>

ParallelBplustree::ParallelBplustree(const int order, const int numThreads) : order(order), numThreads(numThreads), threadPool(numThreads) {
	for (int i = 0; i < numThreads; i++) {
		trees.push_back(new Bplustree(order));
		locks.push_back(new std::mutex);
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
