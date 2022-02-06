#include "program.hpp"
#include <iostream>

Program::Program(const int order, const int threads, const int trees, const bool bloom, const int opDistrLow, const int opDistrHigh, const int buildDistrLow, const int buildDistrHigh) : btree(nullptr), pbtree(new ParallelBplustree(order, threads, trees, bloom)), gen(std::random_device{}()), opDistrLow(opDistrLow), opDistrHigh(opDistrHigh), opDistr(opDistrLow, opDistrHigh), buildDistrLow(buildDistrLow), buildDistrHigh(buildDistrHigh), buildDistr(buildDistrLow, buildDistrHigh) {}

Program::Program(const int order, const int opDistrLow, const int opDistrHigh, const int buildDistrLow, const int buildDistrHigh) : btree(new Bplustree(order)), pbtree(nullptr), gen(std::random_device{}()), opDistrLow(opDistrLow), opDistrHigh(opDistrHigh), opDistr(opDistrLow, opDistrHigh), buildDistrLow(buildDistrLow), buildDistrHigh(buildDistrHigh), buildDistr(buildDistrLow, buildDistrHigh) {}

Program::~Program() {
	if (btree) {
		delete btree;
	}
	else {
		delete pbtree;
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

void Program::insertTest(const int op, const bool show, const bool batch) {
	std::cout << "---Insert performance test---\n";
	buildRandomTree(op, show, true, batch);
}

void Program::buildRandomTree(const int numInserts, const bool show, const bool runAsOp, const bool batch) {
	std::cout << "Tree to be built by " << numInserts << " inserts\n";
	if (runAsOp) {
		std::cout << "Key/value pairs uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	}
	else {
		std::cout << "Key/value pairs uniformly drawn from range [" << buildDistrLow << ", " << buildDistrHigh << "]\n";
	}
	std::uniform_int_distribution<> &distr = runAsOp ? opDistr : buildDistr;
	std::cout << "Building...\n";
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = btree ? buildRandomBplustree(numInserts, distr) : buildRandomParallelBplustree(numInserts, distr, batch);
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

std::chrono::duration<double, std::ratio<1, 1000000000>>::rep Program::buildRandomParallelBplustree(const int numInserts, std::uniform_int_distribution<> &distr, const bool batch) {
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

void Program::searchTest(const int op, const int treeSize, const bool show) {
	std::cout << "---Search performance test---\n";
	buildRandomTree(treeSize, show);
	std::cout << "Search operations to perform: " << op << "\n";
	std::cout << "Keys to search for uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Searching...\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> result = btree ? searchBplustree(op) : searchParallelBplustree(op);
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = std::get<0>(result);
	int hits = std::get<1>(result);
	int misses = std::get<2>(result);
	std::cout << "Search finished in: " << ns / 1000000 << " ms\n";
	std::cout << "Search key hits: " << hits << "\n";
	std::cout << "Search key misses: " << misses << "\n";
	std::cout << "Search performance: " << op / (ns / 1000000000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::searchBplustree(const int op) {
	std::vector<const std::vector<int> *> searchResult;
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

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::searchParallelBplustree(const int op) {
	std::vector<std::future<std::vector<std::future<const std::vector<int> *>>>> searchFutures;
	searchFutures.reserve(op);
	std::vector<std::vector<std::future<const std::vector<int> *>>> searchResult;
	searchResult.reserve(op);
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		searchFutures.push_back(pbtree->search(k));
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		std::vector<std::future<const std::vector<int> *>> temporaryResult = searchFutures[i].get();
		for (int j = 0; j < temporaryResult.size(); j++) {
			temporaryResult[j].wait();
		}
		searchResult.push_back(std::move(temporaryResult));
	}
	auto t3 = std::chrono::high_resolution_clock::now();
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < searchResult[i].size(); j++) {
			if (searchResult[i][j].get()) {
				hits++;
				break;
			}
		}
	}
	std::cout << "Time spent pushing tasks to thread pool: " <<  (t2 - t1).count() / 1000000 << " ms\n";
	std::cout << "Time spent waiting for work to finish: " <<  (t3 - t2).count() / 1000000 << " ms\n";
	return std::make_tuple((t3 - t1).count(), hits, op - hits);
};

void Program::deleteTest(const int op, const int treeSize, const bool show) {
	std::cout << "---Delete performance test---\n";
	buildRandomTree(treeSize, show);
	std::cout << "Delete operations to perform: " << op << "\n";
	std::cout << "Keys to search for uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Deleting...\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> result = btree ? deleteBplustree(op) : deleteParallelBplustree(op);
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = std::get<0>(result);
	int hits = std::get<1>(result);
	int misses = std::get<2>(result);
	std::cout << "Delete finished in: " << ns / 1000000 << " ms\n";
	std::cout << "Delete key hits: " << hits << "\n";
	std::cout << "Delete key misses: " << misses << "\n";
	std::cout << "Delete performance: " << op / (ns / 1000000000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::deleteBplustree(const int op) {
	std::vector<bool> deleteResult;
	deleteResult.reserve(op);
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		deleteResult.push_back(std::move(btree->remove(k)));
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		if (deleteResult[i]) {
			hits++;
		}
	}
	return std::make_tuple((t2 - t1).count(), hits, op - hits);
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> Program::deleteParallelBplustree(const int op) {
	std::vector<std::future<std::vector<std::future<bool>>>> deleteFutures;
		deleteFutures.reserve(op);
		std::vector<std::vector<std::future<bool>>> deleteResult;
		deleteResult.reserve(op);
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			int k = opDistr(gen);
			deleteFutures.push_back(pbtree->remove(k));
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			std::vector<std::future<bool>> temporaryResult = deleteFutures[i].get();
			for (int j = 0; j < temporaryResult.size(); j++) {
				temporaryResult[j].wait();
			}
			deleteResult.push_back(std::move(temporaryResult));
		}
		auto t3 = std::chrono::high_resolution_clock::now();
		std::cout << "Calculating statistics...\n";
		int hits = 0;
		for (int i = 0; i < op; i++) {
			for (int j = 0; j < deleteResult[i].size(); j++) {
				if (deleteResult[i][j].get()) {
					hits++;
					break;
				}
			}
		}
	std::cout << "Time spent pushing tasks to thread pool: " <<  (t2 - t1).count() / 1000000 << " ms\n";
	std::cout << "Time spent waiting for work to finish: " <<  (t3 - t2).count() / 1000000 << " ms\n";
		return std::make_tuple((t3 - t1).count(), hits, op - hits);
}

void Program::updateTest(const int op, const int treeSize, const bool show) {
	std::cout << "---Update performance test---\n";
	buildRandomTree(treeSize, show);
	std::cout << "Update operations to perform: " << op << "\n";
	std::cout << "Keys to update uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Updates performed with single values only\n";
	std::cout << "Updating...\n";
	std::chrono::duration<double, std::ratio<1, 1000000000>>::rep ns = btree ? updateBplustree(op) : updateParallelBplustree(op);
	std::cout << "Update finished in: " << ns / 1000000 << " ms\n";
	std::cout << "Update performance: " << op / (ns / 1000000000) << " ops\n";
}

std::chrono::duration<double, std::ratio<1, 1000000000>>::rep Program::updateBplustree(const int op) {
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		std::vector<int> v = {i};
		btree->update(k, v);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	return (t2 - t1).count();
}

std::chrono::duration<double, std::ratio<1, 1000000000>>::rep Program::updateParallelBplustree(const int op) {
	std::cout << "Value references prepared\n";
	std::vector<int> keys;
	keys.reserve(op);
	std::vector<std::vector<int>> values;
	values.reserve(op);
	for (int i = 0; i < op; i++) {
		keys.push_back(opDistr(gen));
		values.push_back({i});
	}
	std::cout << "Calling update...\n";
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		pbtree->update(keys[i], values[i]);
	}
	pbtree->waitForWorkToFinish();
	auto t2 = std::chrono::high_resolution_clock::now();
	return (t2 - t1).count();
}
