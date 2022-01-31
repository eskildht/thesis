#include "parallelbplustree.hpp"

ParallelBplustree::ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters) : order(order), numThreads(numThreads), threadPool(numThreads), numTrees(numTrees), useBloomFilters(useBloomFilters), accessKey(new AccessKey) {
	for (int i = 0; i < numTrees; i++) {
		trees.push_back(new Bplustree(order));
		treeLocks.push_back(new std::mutex);
		treeNumInsertOp.push_back(0);
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
	bplustree = false;
}

void ParallelBplustree::threadInsert(const int key, const int value, const int treeIndex) {
	std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
	trees[treeIndex]->insert(key, value);
}

void ParallelBplustree::threadInsert(const int key, const std::vector<int> &values, const int treeIndex) {
	std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
	trees[treeIndex]->insert(key, values);
}

std::future<void> ParallelBplustree::insert(const int key, const int value) {
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			if (treeFilters[i]->contains(key)) {
				treeNumInsertOp[i]++;
				return threadPool.push([this](int id, const int key, const int value, const int treeIndex) { this->threadInsert(key, value, treeIndex); }, key, value, i);
			}
		}
	}
	// Distribute keys approximatly evenly when inserting new keys or
	// when not using Bloom filters at all. In any case there is no
	// guarantee that all Bplustrees of ParallelBplustree will hold unique keys.
	// However, by keeping the false positive probability low
	// most keys in the first case will only occur in one of the trees.
	std::vector<int>::iterator it = std::min_element(treeNumInsertOp.begin(), treeNumInsertOp.end());
	const int treeIndex = it - treeNumInsertOp.begin();
	if (useBloomFilters) {
		treeFilters[treeIndex]->insert(key);
	}
	treeNumInsertOp[treeIndex]++;
	return threadPool.push([this](int id, const int key, const int value, const int treeIndex) { this->threadInsert(key, value, treeIndex); }, key, value, treeIndex);
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

bool ParallelBplustree::threadUpdate(const int key, const std::vector<int> &values, const int treeIndex) {
	return trees[treeIndex]->update(key, values);
}

std::vector<int> *ParallelBplustree::threadSearch(const int key, const int treeIndex, AccessKey *accessKey) {
	return trees[treeIndex]->search(key, accessKey);
}

std::vector<std::future<bool>> ParallelBplustree::update(const int key, const std::vector<int> &values) {
	std::vector<std::future<std::vector<int> *>> candidateTreesValues;
	std::vector<int> candidateTreesIndexes;
	std::vector<std::future<bool>> result;
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			if (treeFilters[i]->contains(key)) {
				candidateTreesIndexes.push_back(i);
			}
		}
	}
	else {
		for (int i = 0; i < numTrees; i++) {
			candidateTreesIndexes.push_back(i);
		}
	}
	if (candidateTreesIndexes.size() == 1) {
		result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { return this->threadUpdate(key, values, treeIndex); }, key, values, candidateTreesIndexes[0]));
	}
	else if (candidateTreesIndexes.empty()) {
		std::promise<bool> tmpPromise;
		tmpPromise.set_value(false);
		result.push_back(std::move(tmpPromise.get_future()));
	}
	else {
		for (int i = 0; i < candidateTreesIndexes.size(); i++) {
			candidateTreesValues.push_back(threadPool.push([this](int id, const int key, const int treeIndex, AccessKey *accessKey) { return this->threadSearch(key, treeIndex, accessKey); }, key, candidateTreesIndexes[i], accessKey));
		}
		bool oldValuesFound = false;
		for (int i = 0; i < candidateTreesIndexes.size(); i++) {
			std::vector<int> *oldValues = candidateTreesValues[i].get();
			if (!oldValuesFound) {
				if (oldValues) {
					oldValuesFound = true;
					oldValues->assign(values.begin(), values.end());
					std::promise<bool> tmpPromise;
					tmpPromise.set_value(true);
					result.push_back(std::move(tmpPromise.get_future()));
				}
			}
			else if (oldValues) {
				result.push_back(threadPool.push([this](int id, const int key, const int treeIndex) { return this->threadRemove(key, treeIndex); }, key, candidateTreesIndexes[i]));
			}
		}
	}
	return result;
}

bool ParallelBplustree::threadUpdateOrInsert(const int key, const std::vector<int> &values, const int treeIndex) {
	bool didUpdate;
	{
		std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
		didUpdate = trees[treeIndex]->update(key, values);
	}
	if (!didUpdate) {
		std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
		trees[treeIndex]->insert(key, values);
	}
	return didUpdate;
}

std::vector<std::future<bool>> ParallelBplustree::updateOrInsert(const int key, const std::vector<int> &values) {
	std::vector<std::future<bool>> result;
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
					result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { return this->threadUpdateOrInsert(key, values, treeIndex); }, key, values, i));
				}
				else {
					result.push_back(threadPool.push([this](int id, const int key, const int treeIndex) { return this->threadRemove(key, treeIndex); }, key, i));
				}
			}
		}
		if (!keyWasFound) {
			result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { this->threadInsert(key, values, treeIndex); return false; }, key, values, updateOrInsertTreeSelector));
			updateOrInsertTreeSelector = (updateOrInsertTreeSelector + 1) % numTrees;
		}
		return result;
	}
	result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { return this->threadUpdateOrInsert(key, values, treeIndex); }, key, values, updateOrInsertTreeSelector));
	for (int i = 0; i < numTrees; i++) {
		if (i != updateOrInsertTreeSelector) {
			result.push_back(threadPool.push([this](int id, const int key, const std::vector<int> &values, const int treeIndex) { return this->threadRemove(key, treeIndex); }, key, values, i));
		}
	}
	updateOrInsertTreeSelector = (updateOrInsertTreeSelector + 1) % numTrees;
	return result;
}

bool ParallelBplustree::threadRemove(const int key, const int treeIndex) {
	std::scoped_lock<std::mutex> lock(*treeLocks[treeIndex]);
	return trees[treeIndex]->remove(key);
}

std::vector<std::future<bool>> ParallelBplustree::remove(const int key) {
	std::vector<std::future<bool>> result;
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			if (treeFilters[i]->contains(key)) {
				result.push_back(threadPool.push([this](int id, const int key, const int treeIndex) { return this->threadRemove(key, treeIndex); }, key, i));
				treeNumInsertOp[i]--;
			}
		}
		return result;
	}
	for (int i = 0; i < numTrees; i++) {
		result.push_back(threadPool.push([this](int id, const int key, const int treeIndex) { return this->threadRemove(key, treeIndex); }, key, i));
	}
	return result;
}

void ParallelBplustree::readjustTreeNumInsertOp() {
	treeNumInsertOp = std::move(getTreeNumKeys());
}

std::vector<int> ParallelBplustree::getTreeNumKeys() {
	std::vector<int> result;
	for (int i = 0; i < numTrees; i++) {
		result.push_back(trees[i]->getNumKeysStored());
	}
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

int ParallelBplustree::getOrder() {
	return order;
}

int ParallelBplustree::getNumThreads() {
	return numThreads;
}

int ParallelBplustree::getNumTrees() {
	return numTrees;
}

bool ParallelBplustree::areBloomFiltersUsed() {
	return useBloomFilters;
}
