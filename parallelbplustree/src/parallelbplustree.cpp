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

void ParallelBplustree::threadInsert(const int key, const int value, const int treeIndex) {
	std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
	trees[treeIndex]->insert(key, value);
}

std::future<void> ParallelBplustree::insert(const int key, const int value) {
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			if (treeFilters[i]->contains(key)) {
				treeNumKeys[i]++;
				return threadPool.push([this](int id, const int key, const int value, const int treeIndex) { this->threadInsert(key, value, treeIndex); }, key, value, i);
			}
		}
	}
	// Distribute keys evenly when inserting new keys or when not using
	// Bloom filters at all. In any case there is no guarantee that
	// all Bplustrees of ParallelBplustree will hold unique keys.
	// However, by keeping the false positive probability low
	// most keys in the first case will only occur in one of the trees.
	std::vector<int>::iterator it = std::min_element(treeNumKeys.begin(), treeNumKeys.end());
	treeNumKeys[it - treeNumKeys.begin()]++;
	if (useBloomFilters) {
		treeFilters[it - treeNumKeys.begin()]->insert(key);
	}
	return threadPool.push([this](int id, const int key, const int value, const int treeIndex) { this->threadInsert(key, value, treeIndex); }, key, value, it - treeNumKeys.begin());
}

const std::vector<int> *ParallelBplustree::threadSearch(const int key, const int treeIndex) const {
	return trees[treeIndex]->search(key);
}

std::vector<std::future<const std::vector<int> *>> ParallelBplustree::search(const int key) {
	std::vector<std::future<const std::vector<int> *>> result;
	// Some Bplustrees hopefully won't be searched
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			if (treeFilters[i]->contains(key)) {
				result.push_back(threadPool.push([this](int id, const int key, const int treeIndex) { return this->threadSearch(key, treeIndex); }, key, i));

			}
		}
		return result;
	}
	// All Bplustrees must be searched
	for (int i = 0; i < numTrees; i++) {
		result.push_back(threadPool.push([this](int id, const int key, const int treeIndex) { return this->threadSearch(key, treeIndex); }, key, i));
	}
	return result;
}

void ParallelBplustree::threadUpdateOrInsert(const int key, const std::vector<int> &values, const int treeIndex) {
	std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
	bool didUpdate = trees[treeIndex]->update(key, values);
	if (didUpdate) {
		return;
	}
	else {
		treeNumKeys[treeIndex]++;
		trees[treeIndex]->insert(key, values);
		return;
	}
}

std::vector<std::future<void>> ParallelBplustree::update(const int key, const std::vector<int> &values) {
	std::vector<std::future<void>> result;
	if (useBloomFilters) {
		bool keyWasFound = false;
		for (int i = 0; i < numTrees; i++) {
			// Some trees will hopefully not be operated on.
			// Most likely one tree only if false positive probability is low
			if (treeFilters[i]->contains(key)) {
				// If multiple trees contain the key to be updated, make the key only
				// belong to one tree.
				if (!keyWasFound) {
					keyWasFound = true;
					result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { this->threadUpdateOrInsert(key, values, treeIndex); }, key, values, i));
				}
				else {
					threadPool.push([this](int id, const int key, const int treeIndex) { this->threadRemove(key, treeIndex); }, key, i);
				}
			}
		}
		return result;
	}
	// Remove key from all but one Bplustree, then update or insert 
	// into that one
	for (int i = 1; i < numTrees; i++) {
		result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { this->threadRemove(key, values, treeIndex); }, key, values, i));
	}
	result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { this->threadUpdateOrInsert(key, values, treeIndex); }, key, values, 0));
	return result;
}

void ParallelBplustree::waitForWorkToFinish() {
	// Waits for the thread pool to finish all remaining tasks before
	// stopping the pool. Since pool is stopped, the ParallelBplustree
	// instance from here on does not complete tasks as defined by
	// methods interacting with the pool.
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
