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
	std::cout << "bloom: " << pbtree->areBloomFiltersUsed() << "\n";
}

void Program::insertTest(const int op) {
	std::cout << "---Insert performance test---\n";
	buildRandomTree(op, true);
}

void Program::buildRandomTree(const int numInserts, const bool runAsOp) {
	std::cout << "Tree to be built by " << numInserts << " inserts\n";
	if (runAsOp) {
		std::cout << "Key/value pairs uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	}
	else {
		std::cout << "Key/value pairs uniformly drawn from range [" << buildDistrLow << ", " << buildDistrHigh << "]\n";
	}
	std::uniform_int_distribution<> &distr = runAsOp ? opDistr : buildDistr;
	std::cout << "Building...\n";
	std::chrono::duration<double, std::ratio<1, 1000>>::rep ms = btree ? buildRandomBplustree(numInserts, distr) : buildRandomParallelBplustree(numInserts, distr);
	std::cout << "Build finished in: " << ms << " ms\n";
	std::cout << "Build performance: " << numInserts / (ms / 1000) << " ops\n";
}

std::chrono::duration<double, std::ratio<1, 1000>>::rep Program::buildRandomParallelBplustree(const int numInserts, std::uniform_int_distribution<> &distr) {
	std::vector<std::future<void>> buildFutures;
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < numInserts; i++) {
		int k = distr(gen);
		int v = distr(gen);
		buildFutures.push_back(std::move(pbtree->insert(k, v)));
	}
	for(int i = 0; i < numInserts; i++) {
		buildFutures[i].wait();
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	return (t2 - t1).count();
};

void Program::buildTreeWithUniqueKeys(const int numInserts) {
	std::cout << "Tree to be built by " << numInserts << " inserts\n";
	std::cout << "Key/value pairs inserted consecutively taking values 1-" << numInserts << "\n";
	for (int i = 1; i <= numInserts; i++) {
		pbtree->insert(i, i);
	}
}

void Program::searchTest(const int op, const int treeSize) {
	std::cout << "---Search performance test---\n";
	buildRandomTree(treeSize);
	std::cout << "Search operations to perform: " << op << "\n";
	std::cout << "Keys to search for uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Searching...\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> result = btree ? searchBplustree(op) : searchParallelBplustree(op);
	std::chrono::duration<double, std::ratio<1, 1000>>::rep ms = std::get<0>(result);
	int hits = std::get<1>(result);
	int misses = std::get<2>(result);
	std::cout << "Search finished in: " << ms << " ms\n";
	std::cout << "Search key hits: " << hits << "\n";
	std::cout << "Search key misses: " << misses << "\n";
	std::cout << "Search performance: " << op / (ms / 1000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> Program::searchBplustree(const int op) {
	std::vector<const std::vector<int> *> searchResult;
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		searchResult.push_back(std::move(btree->search(k)));
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		if (searchResult[i]) {
			hits++;
			break;
		}
	}
	return std::make_tuple((t2 - t1).count(), hits, op - hits);
};

std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> Program::searchParallelBplustree(const int op) {
	std::vector<std::vector<std::future<const std::vector<int> *>>> searchFutures;
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		searchFutures.push_back(std::move(pbtree->search(k)));
	}
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < searchFutures[i].size(); j++) {
			searchFutures[i][j].wait();
		}
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < searchFutures[i].size(); j++) {
			if (searchFutures[i][j].get()) {
				hits++;
				break;
			}
		}
	}
	return std::make_tuple((t2 - t1).count(), hits, op - hits);
};

void Program::deleteTest(const int op, const int treeSize) {
	std::cout << "---Delete performance test---\n";
	buildRandomTree(treeSize);
	std::cout << "Delete operations to perform: " << op << "\n";
	std::cout << "Keys to search for uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Deleting...\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> result = btree ? deleteBplustree(op) : deleteParallelBplustree(op);
	std::chrono::duration<double, std::ratio<1, 1000>>::rep ms = std::get<0>(result);
	int hits = std::get<1>(result);
	int misses = std::get<2>(result);
	std::cout << "Delete finished in: " << ms<< " ms\n";
	std::cout << "Delete key hits: " << hits << "\n";
	std::cout << "Delete key misses: " << misses << "\n";
	std::cout << "Delete performance: " << op / (ms / 1000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> Program::deleteBplustree(const int op) {
	std::vector<bool> deleteResult;
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		deleteResult.push_back(std::move(btree->remove(k)));
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		if (deleteResult[i]) {
			hits++;
			break;
		}
	}
	return std::make_tuple((t2 - t1).count(), hits, op - hits);
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> Program::deleteParallelBplustree(const int op) {
	std::vector<std::vector<std::future<bool>>> deleteFutures;
		std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < op; i++) {
			int k = opDistr(gen);
			deleteFutures.push_back(std::move(pbtree->remove(k)));
		}
		for (int i = 0; i < op; i++) {
			for (int j = 0; j < deleteFutures[i].size(); j++) {
				deleteFutures[i][j].wait();
			}
		}
		std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::cout << "Calculating statistics...\n";
		int hits = 0;
		for (int i = 0; i < op; i++) {
			for (int j = 0; j < deleteFutures[i].size(); j++) {
				if (deleteFutures[i][j].get()) {
					hits++;
					break;
				}
			}
		}
		return std::make_tuple((t2 - t1).count(), hits, op - hits);
}

void Program::updateTest(const int op, const int treeSize) {
	std::cout << "---Update performance test---\n";
	buildRandomTree(treeSize);
	std::cout << "Update operations to perform: " << op << "\n";
	std::cout << "Keys to update uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Updates performed with single values only\n";
	std::cout << "Updating...\n";
	std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> result = btree ? updateBplustree(op) : updateParallelBplustree(op);
	std::chrono::duration<double, std::ratio<1, 1000>>::rep ms = std::get<0>(result);
	int hits = std::get<1>(result);
	int misses = std::get<2>(result);
	std::cout << "Update finished in: " << ms << " ms\n";
	std::cout << "Successful updates: " << hits << "\n";
	std::cout << "Unsuccessful updates (key not present in tree): " << misses << "\n";
	std::cout << "Update performance: " << op / (ms / 1000) << " ops\n";
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> Program::updateBplustree(const int op) {
	std::vector<bool> updateResult;
	updateResult.reserve(op);
	std::chrono::duration<double, std::milli> ms_double(0);
	std::chrono::steady_clock::time_point t1;
	std::chrono::steady_clock::time_point t2;
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		std::vector<int> v = {i};
		t1 = std::chrono::high_resolution_clock::now();
		updateResult.push_back(std::move(btree->update(k, v)));
		t2 = std::chrono::high_resolution_clock::now();
		ms_double += t2 - t1;
	}
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		if (updateResult[i]) {
			hits++;
		}
	}
	return std::make_tuple(ms_double.count(), hits, op - hits);
}

std::tuple<std::chrono::duration<double, std::ratio<1, 1000>>::rep, int, int> Program::updateParallelBplustree(const int op) {
	std::vector<std::vector<std::future<bool>>> updateFutures;
	updateFutures.reserve(op);
	std::chrono::duration<double, std::milli> ms_double(0);
	std::chrono::steady_clock::time_point t1;
	std::chrono::steady_clock::time_point t2;
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		std::vector<int> v = {i};
		t1 = std::chrono::high_resolution_clock::now();
		updateFutures.push_back(std::move(pbtree->update(k, v)));
		t2 = std::chrono::high_resolution_clock::now();
		ms_double += t2 - t1;
	}
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < updateFutures[i].size(); j++) {
			t1 = std::chrono::high_resolution_clock::now();
			updateFutures[i][j].wait();
			t2 = std::chrono::high_resolution_clock::now();
			ms_double += t2 - t1;
		}
	}
	std::cout << "Calculating statistics...\n";
	int hits = 0;
	for (int i = 0; i < op; i++) {
		if (updateFutures[i].size() == 1) {
			if (updateFutures[i][0].get()) {
				hits++;
			}
		}
		else if (updateFutures[i].size() > 1) {
			hits++;
		}
	}
	return std::make_tuple(ms_double.count(), hits, op - hits);
}

void Program::updateOrInsertTest(const int op, const int treeSize) {
	std::cout << "---Update or insert performance test---\n";
	buildRandomTree(treeSize);
	std::cout << "Update operations to perform: " << op << "\n";
	std::cout << "Keys to update uniformly drawn from range [" << opDistrLow << ", " << opDistrHigh << "]\n";
	std::cout << "Updates performed with single values only\n";
	std::cout << "Retrieving number of unique keys in each sub Bplustree for later use...\n";
	std::vector<int> beforeTreeNumKeys = pbtree->getTreeNumKeys();
	std::cout << "Updating...\n";
	std::vector<std::vector<std::future<bool>>> updateFutures;
	updateFutures.reserve(op);
	std::chrono::duration<double, std::milli> ms_double(0);
	std::chrono::steady_clock::time_point t1;
	std::chrono::steady_clock::time_point t2;
	for (int i = 0; i < op; i++) {
		int k = opDistr(gen);
		std::vector<int> v = {i};
		t1 = std::chrono::high_resolution_clock::now();
		updateFutures.push_back(std::move(pbtree->updateOrInsert(k, v)));
		t2 = std::chrono::high_resolution_clock::now();
		ms_double += t2 - t1;
	}
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < updateFutures[i].size(); j++) {
			t1 = std::chrono::high_resolution_clock::now();
			updateFutures[i][j].wait();
			t2 = std::chrono::high_resolution_clock::now();
			ms_double += t2 - t1;
		}
	}
	std::cout << "Calculating statistics...\n";
	std::cout << "Number of unique keys in each tree before update: [";
	for (int i = 0; i < beforeTreeNumKeys.size() - 1; i++) {
		std::cout << beforeTreeNumKeys[i] << ", ";
	}
	std::cout << beforeTreeNumKeys[beforeTreeNumKeys.size() - 1] << "]\n";
	std::cout << "Number of unique keys in each tree after update: [";
	std::vector<int> afterTreeNumKeys = pbtree->getTreeNumKeys();
	for (int i = 0; i < afterTreeNumKeys.size() - 1; i++) {
		std::cout << afterTreeNumKeys[i] << ", ";
	}
	std::cout << afterTreeNumKeys[afterTreeNumKeys.size() - 1] << "]\n";
	int hits = 0;
	for (int i = 0; i < updateFutures.size(); i++) {
		if (updateFutures[i][0].get()) {
			hits++;
		}
	}
	int misses = op - hits;
	std::cout << "Update finished in: " << ms_double.count() << " ms\n";
	std::cout << "Successful updates: " << hits << "\n";
	std::cout << "Unsuccessful updates leading to inserts: " << misses << "\n";
	std::cout << "Updateorinsert performance: " << op / (ms_double.count() / 1000) << " ops\n";
}
