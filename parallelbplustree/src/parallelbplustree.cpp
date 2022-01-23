#include "parallelbplustree.hpp"

ParallelBplustree::ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters) : order(order), numThreads(numThreads), threadPool(numThreads), numTrees(numTrees), useBloomFilters(useBloomFilters) {
	for (int i = 0; i < numTrees; i++) {
		trees.push_back(new Bplustree(order));
		treeLocks.push_back(new std::mutex);
		treeNumKeys.push_back(0);
	}
	if (useBloomFilters) {
		bloom_parameters parameters;
		// Maximum false_positive_probability set to 1 in 1000000
		// for each filter.
		parameters.projected_element_count = 1000000;
		parameters.false_positive_probability = 0.000001;
		parameters.compute_optimal_parameters();
		for (int i = 0; i < numTrees; i++) {
			treeFilters.push_back(new bloom_filter(parameters));
		}
	}
}

void ParallelBplustree::threadInsert(int id, const int key, const int value, const int treeIndex) {
	std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
	trees[treeIndex]->insert(key, value);
}

std::future<void> ParallelBplustree::insert(const int key, const int value) {
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			if (treeFilters[i]->contains(key)) {
				treeNumKeys[i]++;
				return threadPool.push([this](int id, const int key, const int value, const int treeIndex) { this->threadInsert(id, key, value, treeIndex); }, key, value, i);
			}
		}
	}
	// Distribute keys evenly when inserting new keys or when not using
	// Bloom filters at all. In the latter case there is no guarantee that
	// all Bplustrees of ParallelBplustree will hold unique keys.
	std::vector<int>::iterator it = std::min_element(treeNumKeys.begin(), treeNumKeys.end());
	treeNumKeys[it - treeNumKeys.begin()]++;
	if (useBloomFilters) {
		treeFilters[it - treeNumKeys.begin()]->insert(key);
	}
	return threadPool.push([this](int id, const int key, const int value, const int treeIndex) { this->threadInsert(id, key, value, treeIndex); }, key, value, it - treeNumKeys.begin());
}
void ParallelBplustree::waitForWorkToFinish() {
	threadPool.stop(true);
}

void ParallelBplustree::show() {
	for (int i = 0; i < numTrees; i++) {
		trees[i]->show();
	}
}

ParallelBplustree::~ParallelBplustree() {
	threadPool.stop();
	for (int i = 0; i < numTrees; i++) {
		delete trees[i];
		delete treeLocks[i];
		if (useBloomFilters) {
			delete treeFilters[i];
		}
	}
}
