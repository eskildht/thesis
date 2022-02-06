#include "parallelbplustree.hpp"
#include <random>

ParallelBplustree::ParallelBplustree(const int order, const int numThreads, const int numTrees, const bool useBloomFilters) : order(order), numThreads(numThreads), threadPool(numThreads), numTrees(numTrees), useBloomFilters(useBloomFilters) {
	for (int i = 0; i < numTrees; i++) {
		trees.push_back(new Bplustree(order));
		treeLocks.push_back(new std::shared_mutex);
		treeFilterLocks.push_back(new std::shared_mutex);
	}
	if (useBloomFilters) {
		bloom_parameters parameters;
		parameters.projected_element_count = 1000000;
		parameters.false_positive_probability = 0.000001;
		parameters.compute_optimal_parameters();
		for (int i = 0; i < numTrees; i++) {
			treeFilters.push_back(new bloom_filter(parameters));
		}
	}
}

void ParallelBplustree::threadInsert(const int key, const int value) {
	static thread_local std::mt19937 gen;
	static thread_local std::uniform_int_distribution<int> distr(0, numTrees - 1);
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			std::shared_lock<std::shared_mutex> treeFilterReadLock(*treeFilterLocks[i]);
			if (treeFilters[i]->contains(key)) {
				treeFilterReadLock.unlock();
				{
					std::unique_lock<std::shared_mutex> treeWriteLock(*treeLocks[i]);
					trees[i]->insert(key, value);
				}
				{
					std::unique_lock<std::shared_mutex> treeFilterWriteLock(*treeFilterLocks[i]);
					treeFilters[i]->insert(key);
				}
				return;
			}
		}
		std::uniform_int_distribution<int> distr(0, numTrees - 1);
		const int treeIndex	= distr(gen);
		{
			std::unique_lock<std::shared_mutex> treeWriteLock(*treeLocks[treeIndex]);
			trees[treeIndex]->insert(key, value);
		}
		{
			std::unique_lock<std::shared_mutex> treeFilterWriteLock(*treeFilterLocks[treeIndex]);
			treeFilters[treeIndex]->insert(key);
		}
	}
	else {
		const int treeIndex	= distr(gen);
		std::unique_lock<std::shared_mutex> treeWriteLock(*treeLocks[treeIndex]);
		trees[treeIndex]->insert(key, value);
	}
}

void ParallelBplustree::insert(const int key, const int value) {
	threadPool.push_task([=, this] { threadInsert(key, value); });
}

void ParallelBplustree::threadInsert(std::vector<int>::iterator keysSplitBegin, std::vector<int>::iterator keysSplitEnd, std::vector<int>::iterator valuesSplitBegin, const int treeIndex) {
	if (treeIndex > -1) {
		std::unique_lock<std::shared_mutex> treeWriteLock(*treeLocks[treeIndex]);
		for(std::vector<int>::iterator keysSplitIt = keysSplitBegin; keysSplitIt != keysSplitEnd; keysSplitIt++, valuesSplitBegin++) {
			trees[treeIndex]->insert(*keysSplitIt, *valuesSplitBegin);
		}
	}
	else {
		for(std::vector<int>::iterator keysSplitIt = keysSplitBegin; keysSplitIt != keysSplitEnd; keysSplitIt++, valuesSplitBegin++) {
			threadInsert(*keysSplitIt, *valuesSplitBegin);
		}
	}
}

void ParallelBplustree::insert(std::vector<int> &keys, std::vector<int> &values) {
	if (keys.size() != values.size()) {
		throw "keys.size() and values.size() must be equal\n";
	}
	std::vector<int>::iterator keysIt = keys.begin();
	std::vector<int>::iterator valuesIt = values.begin();

	if (useBloomFilters) {
		if (keys.size() < numThreads) {
			throw "Not enough pairs passed, use insert(const int key, const int value) instead\n";
		}
		const size_t splitSize = keys.size() / numThreads;
		for (int i = 0; i < (numThreads - 1); i++) {
			threadPool.push_task([=, this] { threadInsert(keysIt + i*splitSize, keysIt + (i+1)*splitSize, valuesIt + i*splitSize); });
		}
		std::vector<int>::iterator keysItEnd = keys.end();
		threadPool.push_task([=, this] { threadInsert(keysIt + (numThreads - 1)*splitSize, keysItEnd, valuesIt + (numThreads - 1)*splitSize); });
	}
	else {
		if (keys.size() < numTrees) {
			throw "Not enough pairs passed, use insert(const int key, const int value) instead\n";
		}
		const size_t splitSize = keys.size() / numTrees;
		for (int i = 0; i < (numTrees - 1); i++) {
			threadPool.push_task([=, this] { threadInsert(keysIt + i*splitSize, keysIt + (i+1)*splitSize, valuesIt + i*splitSize, i); });
		}
		std::vector<int>::iterator keysItEnd = keys.end();
		threadPool.push_task([=, this] { threadInsert(keysIt + (numTrees - 1)*splitSize, keysItEnd, valuesIt + (numTrees - 1)*splitSize, numTrees - 1); });
	}
}

void ParallelBplustree::threadSearchCoordinator(const int key, std::promise<std::vector<std::future<const std::vector<int> *>>> *prom) {
	std::vector<std::future<const std::vector<int> *>> result;
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			std::shared_lock<std::shared_mutex> treeFilterReadLock(*treeFilterLocks[i]);
			if (treeFilters[i]->contains(key)) {
				treeFilterReadLock.unlock();
				result.push_back(threadPool.submit([=, this] { return threadSearch(key, i); }));
			}
		}
	}
	else {
		for (int i = 0; i < numTrees; i++) {
			result.push_back(threadPool.submit([=, this] { return threadSearch(key, i); }));
		}
	}
	prom->set_value(std::move(result));
	delete prom;
}

const std::vector<int> *ParallelBplustree::threadSearch(const int key, const int treeIndex) const {
	std::shared_lock<std::shared_mutex> treeReadLock(*treeLocks[treeIndex]);
	return trees[treeIndex]->search(key);
}

std::future<std::vector<std::future<const std::vector<int> *>>> ParallelBplustree::search(const int key) {
	std::promise<std::vector<std::future<const std::vector<int> *>>> *prom = new std::promise<std::vector<std::future<const std::vector<int> *>>>;
	std::future<std::vector<std::future<const std::vector<int> *>>> fut = prom->get_future();
	threadPool.push_task([=, this] () mutable { threadSearchCoordinator(key, prom); });
	return fut;
}

bool ParallelBplustree::threadUpdate(const int key, const std::vector<int> &values, const int treeIndex) {
	std::unique_lock<std::shared_mutex> treeWriteLock(*treeLocks[treeIndex]);
	return trees[treeIndex]->update(key, values, true);
}


void ParallelBplustree::threadUpdateCoordinator(const int key, const std::vector<int> &values) {
	static thread_local std::mt19937 gen;
	static thread_local std::uniform_int_distribution<int> distr(0, numTrees - 1);
	if (useBloomFilters) {
		bool keyWasFoundInFilter = false;
		for (int i = 0; i < numTrees; i++) {
			std::shared_lock<std::shared_mutex> treeFiltersReadLock(*treeFilterLocks[i]);
			if (treeFilters[i]->contains(key)) {
				treeFiltersReadLock.unlock();
				if (!keyWasFoundInFilter) {
					threadPool.push_task([=, &values, this] { threadUpdate(key, values, i); });
					keyWasFoundInFilter = true;
				}
				else {
					threadPool.push_task([=, &values, this] { threadRemove(key, i); });
				}
			}
		}
		if (!keyWasFoundInFilter) {
			int treeToUpdateOrInsert = distr(gen);
			threadPool.push_task([=, &values, this] { threadUpdate(key, values, treeToUpdateOrInsert); });
		}
	}
	else {
		int treeToUpdateOrInsert = distr(gen);
		threadPool.push_task([=, &values, this] { threadUpdate(key, values, treeToUpdateOrInsert); });
		for (int i = 0; i < numTrees; i++) {
			if (i != treeToUpdateOrInsert) {
				threadPool.push_task([=, this] { threadRemove(key, i); });
			}
		}
	}
}

void ParallelBplustree::update(const int key, const std::vector<int> &values) {
	threadPool.push_task([=, &values, this] { threadUpdateCoordinator(key, values); });
}

bool ParallelBplustree::threadRemove(const int key, const int treeIndex) {
	std::unique_lock<std::shared_mutex> treeWriteLock(*treeLocks[treeIndex]);
	return trees[treeIndex]->remove(key);
}

void ParallelBplustree::threadRemoveCoordinator(const int key, std::promise<std::vector<std::future<bool>>> *prom) {
	std::vector<std::future<bool>> result;
	if (useBloomFilters) {
		for (int i = 0; i < numTrees; i++) {
			std::shared_lock<std::shared_mutex> treeFilterReadLock(*treeFilterLocks[i]);
			if (treeFilters[i]->contains(key)) {
				treeFilterReadLock.unlock();
				result.push_back(threadPool.submit([=, this] { return threadRemove(key, i); }));
			}
		}
	}
	else {
		for (int i = 0; i < numTrees; i++) {
			result.push_back(threadPool.submit([=, this] { return threadRemove(key, i); }));
		}
	}
	prom->set_value(std::move(result));
	delete prom;
}

std::future<std::vector<std::future<bool>>> ParallelBplustree::remove(const int key) {
	std::promise<std::vector<std::future<bool>>> *prom = new std::promise<std::vector<std::future<bool>>>;
	std::future<std::vector<std::future<bool>>> fut = prom->get_future();
	threadPool.push_task([=, this] () mutable { threadRemoveCoordinator(key, prom); });
	return fut;
}

//void ParallelBplustree::readjustTreeNumInsertOp() {
//	treeNumInsertOp = std::move(getTreeNumKeys());
//}

std::vector<int> ParallelBplustree::getTreeNumKeys() {
	std::vector<int> result;
	for (int i = 0; i < numTrees; i++) {
		result.push_back(trees[i]->getNumKeysStored());
	}
	return result;
}

void ParallelBplustree::waitForWorkToFinish() {
	threadPool.wait_for_tasks();
}

void ParallelBplustree::show() {
	for (int i = 0; i < numTrees; i++) {
		trees[i]->show();
	}
}

ParallelBplustree::~ParallelBplustree() {
	for (int i = 0; i < numTrees; i++) {
		delete trees[i];
		delete treeLocks[i];
		if (useBloomFilters) {
			delete treeFilters[i];
			delete treeFilterLocks[i];
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

void ParallelBplustree::pauseThreadPool() {
	threadPool.paused = true;
}

void ParallelBplustree::resumeThreadPool() {
	threadPool.paused = false;
}
