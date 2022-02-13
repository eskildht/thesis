#include "program.hpp"
#include <iostream>

Program::Program(
		const std::string treeType,
		const int order,
		const int threads,
		const int trees,
		const bool bloom,
		const int op,
		const int opDistrLow,
		const int opDistrHigh,
		const int buildDistrLow,
		const int buildDistrHigh,
		const bool show,
		const bool batch,
		const int treeSize,
		const std::string test
		) :
	gen(std::random_device{}()),
	op(op),
	opDistrLow(opDistrLow),
	opDistrHigh(opDistrHigh),
	opDistr(opDistrLow, opDistrHigh),
	buildDistrLow(buildDistrLow),
	buildDistrHigh(buildDistrHigh),
	buildDistr(buildDistrLow, buildDistrHigh),
	show(show),
	batch(batch),
	treeSize(treeSize),
	test(test) {
		if (treeType == "basic") {
			pbtree = nullptr;
			btree = new Bplustree(order);
		}
		else {
			pbtree = new ParallelBplustree(order, threads, trees, bloom);
			btree = nullptr;
		}
	}

Program::~Program() {
	if (btree) {
		delete btree;
	}
	else {
		delete pbtree;
	}
}

void Program::runTest() {
	printTreeInfo();
	if (test == "delete") {
		deleteTest();
	}
	else if (test == "insert") {
		insertTest();
	}
	else if (test == "search") {
		searchTest();
	}
	else if (test == "update") {
		updateTest();
	}
}

void Program::printTreeInfo() {
	btree ? printBplustreeInfo() : printParallelBplustreeInfo();
}

void Program::printBplustreeInfo() {
	std::cout << "---Bplustree information--\n";
	std::cout << "order: " << btree->getOrder() << "\n";
}

void Program::printParallelBplustreeInfo() {
	std::cout << "---ParallelBplustree information--\n";
	std::cout << "order: " << pbtree->getOrder() << "\n";
	std::cout << "trees: " << pbtree->getNumTrees() << "\n";
	std::cout << "threads: " << pbtree->getNumThreads() << "\n";
	std::cout << "bloom: " << pbtree->areBloomFiltersUsed() << "\n";
}

void Program::insertTest() {
	std::cout << "---Insert performance test---\n";
	buildRandomTree(true);
}

void Program::buildRandomTree(const bool runAsOp) {
	int numInserts = runAsOp ? op : treeSize;
	int distrHigh = runAsOp ? opDistrHigh : buildDistrHigh;
	int distrLow = runAsOp ? opDistrLow : buildDistrLow;
	std::uniform_int_distribution<> &distr = runAsOp ? opDistr : buildDistr;
	std::cout << "Tree to be built by " << numInserts << " inserts\n";
	std::cout << "Key/value pairs uniformly drawn from range [" << distrLow << ", " << distrHigh << "]\n";
	std::cout << "Building...\n";
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = btree ? buildRandomBplustree(numInserts, distr) : buildRandomParallelBplustree(numInserts, distr);
	std::cout << "Build finished in: " << ns / 1000000 << " ms\n";
	std::cout << "Build performance: " << numInserts / (ns / 1000000000) << " ops\n";
	if (show) {
		if (numInserts <= 1000) {
			std::cout << "---Tree print---\n";
			btree ? btree->show() : pbtree->show();
			std::cout << "---Tree print end---\n";
		}
	}
}

std::chrono::duration<double, std::ratio<1, 1000000000>>::rep Program::buildRandomBplustree(const int numInserts, std::uniform_int_distribution<> &distr) {
	auto t1 = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < numInserts; i++) {
		int k = distr(gen);
		int v = distr(gen);
		btree->insert(k, v);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	return (t2 - t1).count();
}

std::chrono::duration<double, std::ratio<1, 1000000000>>::rep Program::buildRandomParallelBplustree(const int numInserts, std::uniform_int_distribution<> &distr) {
	auto t1 = std::chrono::high_resolution_clock::now();
	auto t2 = std::chrono::high_resolution_clock::now();
	std::vector<int> keys;
	std::vector<int> values;
	if (batch) {
		keys.reserve(numInserts);
		values.reserve(numInserts);
		for(int i = 0; i < numInserts; i++) {
			keys.push_back(distr(gen));
			values.push_back(distr(gen));
		}
		t1 = std::chrono::high_resolution_clock::now();
		pbtree->insert(keys, values);
		t2 = std::chrono::high_resolution_clock::now();
	}
	else {
		t1 = std::chrono::high_resolution_clock::now();
		for(int i = 0; i < numInserts; i++) {
			int k = distr(gen);
			int v = distr(gen);
			pbtree->insert(k, v);
		}
		t2 = std::chrono::high_resolution_clock::now();
	}
	pbtree->waitForWorkToFinish();
	auto t3 = std::chrono::high_resolution_clock::now();
	std::cout << "Time spent pushing tasks to thread pool: " <<  (t2 - t1).count() / 1000000 << " ms\n";
	std::cout << "Time spent waiting for work to finish: " <<  (t3 - t2).count() / 1000000 << " ms\n";
	return (t3 - t1).count();
};

void Program::searchTest() {
	std::cout << "---Search performance test---\n";
	buildRandomTree();
	std::cout << "Search operations to perform: " << op << "\n";
	std::cout << "Keys to search for uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> result = btree ? searchBplustree() : searchParallelBplustree();
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = std::get<0>(result);
	int hits = std::get<1>(result);
	int misses = std::get<2>(result);
	std::cout << "Search finished in: " << ns / 1000000 << " ms\n";
	std::cout << "Search key hits: " << hits << "\n";
	std::cout << "Search key misses: " << misses << "\n";
	std::cout << "Search performance: " << op / (ns / 1000000000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::searchBplustree() {
	std::vector<const std::vector<int> *> searchResult;
	std::cout << "Calling search...\n";
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		searchResult.push_back(std::move(btree->search(k)));
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		if (searchResult[i]) {
			hits++;
		}
	}
	return std::make_tuple((t2 - t1).count(), hits, op - hits);
};

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::searchParallelBplustree() {
	auto t1 = std::chrono::high_resolution_clock::now();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto t3 = std::chrono::high_resolution_clock::now();
	int hits = 0;
	if (batch) {
		std::cout << "Generating keys for batch search...\n";
		std::vector<int> keys;
		keys.reserve(op);
		for (int i = 0; i < op; i++) {
			keys.push_back(opDistr(gen));
		}
		std::cout << "Calling search...\n";
		t1 = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<const std::vector<int> *>> result = pbtree->search(keys);
		t2 = std::chrono::high_resolution_clock::now();
		pbtree->waitForWorkToFinish();
		t3 = std::chrono::high_resolution_clock::now();
		std::cout << "Calculating statistics...\n";
		for (int i = 0; i < result.size(); i++) {
			for (int j = 0; j < result[i].size(); j++) {
				if (result[i][j]) {
					hits++;
					break;
				}
			}
		}
	}
	else {
		std::vector<std::future<std::vector<std::future<const std::vector<int> *>>>> searchFutures;
		searchFutures.reserve(op);
		std::vector<std::vector<std::future<const std::vector<int> *>>> searchResult;
		searchResult.reserve(op);
		std::cout << "Calling search...\n";
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			int k = opDistr(gen);
			searchFutures.push_back(pbtree->search(k));
		}
		t2 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			std::vector<std::future<const std::vector<int> *>> temporaryResult = searchFutures[i].get();
			for (int j = 0; j < temporaryResult.size(); j++) {
				temporaryResult[j].wait();
			}
			searchResult.push_back(std::move(temporaryResult));
		}
		t3 = std::chrono::high_resolution_clock::now();
		std::cout << "Calculating statistics...\n";
		for (int i = 0; i < op; i++) {
			for (int j = 0; j < searchResult[i].size(); j++) {
				if (searchResult[i][j].get()) {
					hits++;
					break;
				}
			}
		}
	}
	std::cout << "Time spent pushing tasks to thread pool: " <<	(t2 - t1).count() / 1000000 << " ms\n";
	std::cout << "Time spent waiting for work to finish: " <<	(t3 - t2).count() / 1000000 << " ms\n";
	return std::make_tuple((t3 - t1).count(), hits, op - hits);
};

void Program::deleteTest() {
	std::cout << "---Delete performance test---\n";
	buildRandomTree();
	std::cout << "Delete operations to perform: " << op << "\n";
	std::cout << "Keys to search for uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> result = btree ? deleteBplustree() : deleteParallelBplustree();
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = std::get<0>(result);
	int oldNumKeys = std::get<1>(result);
	int newNumKeys = std::get<2>(result);
	std::cout << "Delete finished in: " << ns / 1000000 << " ms\n";
	std::cout << "Tree size before delete: " << oldNumKeys << "\n";
	std::cout << "Tree size after delete: " << newNumKeys << "\n";
	std::cout << "Delete performance: " << op / (ns / 1000000000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::deleteBplustree() {
	int oldNumKeys = btree->getNumKeysStored();
	std::cout << "Calling remove...\n";
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		btree->remove(opDistr(gen));
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	int newNumKeys = btree->getNumKeysStored();
	return std::make_tuple((t2 - t1).count(), oldNumKeys, newNumKeys);
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::deleteParallelBplustree() {
	std::vector<std::future<std::vector<std::future<bool>>>> deleteFutures;
	deleteFutures.reserve(op);
	auto t1 = std::chrono::high_resolution_clock::now();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto t3 = std::chrono::high_resolution_clock::now();
	int oldNumKeys = 0;
	for (int num : pbtree->getTreeNumKeys()) {
		oldNumKeys += num;
	}
	if (batch) {
		std::cout << "Generating keys for batch delete...\n";
		std::vector<int> keys;
		keys.reserve(op);
		for (int i = 0; i < op; i++) {
			keys.push_back(opDistr(gen));
		}
		std::cout << "Calling remove...\n";
		t1 = std::chrono::high_resolution_clock::now();
		pbtree->remove(keys);
		t2 = std::chrono::high_resolution_clock::now();
		pbtree->waitForWorkToFinish();
		t3 = std::chrono::high_resolution_clock::now();
	}
	else {
		std::cout << "Calling remove...\n";
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			deleteFutures.push_back(pbtree->remove(opDistr(gen)));
		}
		t2 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			std::vector<std::future<bool>> temporaryResult = deleteFutures[i].get();
			for (int j = 0; j < temporaryResult.size(); j++) {
				temporaryResult[j].wait();
			}
		}
		t3 = std::chrono::high_resolution_clock::now();
	}
	int newNumKeys = 0;
	for (int num : pbtree->getTreeNumKeys()) {
		newNumKeys += num;
	}
	std::cout << "Time spent pushing tasks to thread pool: " <<  (t2 - t1).count() / 1000000 << " ms\n";
	std::cout << "Time spent waiting for work to finish: " <<  (t3 - t2).count() / 1000000 << " ms\n";
	return std::make_tuple((t3 - t1).count(), oldNumKeys, newNumKeys);
}

void Program::updateTest() {
	std::cout << "---Update performance test---\n";
	buildRandomTree();
	std::cout << "Update operations to perform: " << op << "\n";
	std::cout << "Keys to update uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Updates performed with single values only\n";
	std::cout << "insertIfNotFound = true for all updates\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> result = btree ? updateBplustree() : updateParallelBplustree();
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = std::get<0>(result);
	int oldNumKeys = std::get<1>(result);
	int newNumKeys = std::get<2>(result);
	std::cout << "Update finished in: " << ns / 1000000 << " ms\n";
	std::cout << "Tree size before update: " << oldNumKeys << "\n";
	std::cout << "Tree size after update: " << newNumKeys << "\n";
	std::cout << "Update performance: " << op / (ns / 1000000000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::updateBplustree() {
	int oldNumKeys = btree->getNumKeysStored();
	std::cout << "Calling update...\n";
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		std::vector<int> v = {i};
		btree->update(k, v, true);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	int newNumKeys = btree->getNumKeysStored();
	return std::make_tuple((t2 - t1).count(), oldNumKeys, newNumKeys);
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::updateParallelBplustree() {
	std::cout << "Generating keys and values...\n";
	std::vector<int> keys;
	keys.reserve(op);
	std::vector<std::vector<int>> values;
	values.reserve(op);
	for (int i = 0; i < op; i++) {
		keys.push_back(opDistr(gen));
		values.push_back({i});
	}
	int oldNumKeys = 0;
	for (int num : pbtree->getTreeNumKeys()) {
		oldNumKeys += num;
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto t3 = std::chrono::high_resolution_clock::now();
	std::cout << "Calling update...\n";
	if (batch) {
		t1 = std::chrono::high_resolution_clock::now();
		pbtree->update(keys, values);
		t2 = std::chrono::high_resolution_clock::now();
	}
	else {
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			pbtree->update(keys[i], values[i]);
		}
		t2 = std::chrono::high_resolution_clock::now();
	}
	pbtree->waitForWorkToFinish();
	t3 = std::chrono::high_resolution_clock::now();
	int newNumKeys = 0;
	for (int num : pbtree->getTreeNumKeys()) {
		newNumKeys += num;
	}
	std::cout << "Time spent pushing tasks to thread pool: " <<  (t2 - t1).count() / 1000000 << " ms\n";
	std::cout << "Time spent waiting for work to finish: " <<  (t3 - t2).count() / 1000000 << " ms\n";
	return std::make_tuple((t3 - t1).count(), oldNumKeys, newNumKeys);
}
