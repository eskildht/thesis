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
	std::cout << "threads: " << pbtree->getNumThreads() << "\n";
	std::cout << "trees: " << pbtree->getNumTrees() << "\n";
	std::cout << "bloom: " << pbtree->areBloomFiltersUsed() << "\n";
}

void Program::insertTest(const int op, const bool runAsOp) {
	std::cout << "---Insert performance test---\n";
	runAsOp ? buildRandomTree(op, true) : buildRandomTree(op, false);
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

std::chrono::duration<double, std::ratio<1, 1000>>::rep Program::buildRandomBplustree(const int numInserts, std::uniform_int_distribution<> &distr) {
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < numInserts; i++) {
		int k = distr(gen);
		int v = distr(gen);
		btree->insert(k, v);
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	return (t2 - t1).count();
};

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

void Program::searchTest(const int op) {
	std::cout << "---Search performance test---\n";
	int distrLow = 1;
	int distrHigh = 500000;
	buildRandomTree(1000000, distrLow, distrHigh);
	std::cout << "Search operations to perform: " << op << "\n";
	std::cout << "Keys to search for uniformly drawn from range [" << distrLow << ", " << distrHigh << "]\n";
	std::cout << "Searching...\n";
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
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	int hits = 0;
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < searchFutures[i].size(); j++) {
			if (searchFutures[i][j].get()) {
				hits++;
				break;
			}
		}
	}
	int misses = op - hits;
	std::cout << "Search finished in: " << ms_double.count() << " ms\n";
	std::cout << "Search key hits: " << hits << "\n";
	std::cout << "Search key misses: " << misses << "\n";
	std::cout << "Search performance: " << op / (ms_double.count() / 1000) << " ops\n";
}

void Program::deleteTest(const int op) {
	std::cout << "---Delete performance test---\n";
	int distrLow = 1;
	int distrHigh = 500000;
	buildRandomTree(1000000, distrLow, distrHigh);
	std::cout << "Delete operations to perform: " << op << "\n";
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(distrLow, distrHigh);
	std::cout << "Keys to search for uniformly drawn from range [" << distrLow << ", " << distrHigh << "]\n";
	std::cout << "Deleting...\n";
	std::vector<std::vector<std::future<bool>>> deleteFutures;
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = distr(gen);
		deleteFutures.push_back(std::move(pbtree->remove(k)));
	}
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < deleteFutures[i].size(); j++) {
			deleteFutures[i][j].wait();
		}
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::cout << "Calculating statistics...\n";
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	int hits = 0;
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < deleteFutures[i].size(); j++) {
			if (deleteFutures[i][j].get()) {
				hits++;
				break;
			}
		}
	}
	int misses = op - hits;
	std::cout << "Delete finished in: " << ms_double.count() << " ms\n";
	std::cout << "Delete key hits: " << hits << "\n";
	std::cout << "Delete key misses: " << misses << "\n";
	std::cout << "Delete performance: " << op / (ms_double.count() / 1000) << " ops\n";
}


void Program::updateTest(const int op) {
	std::cout << "---Update performance test---\n";
	int keyDistrLow = 1;
	int keyDistrHigh = 500000;
	int valuesDistrLow = 1;
	int valuesDistrHigh = 5;
	buildRandomTree(1000000, keyDistrLow, keyDistrHigh);
	std::cout << "Update operations to perform: " << op << "\n";
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> keyDist(keyDistrLow, keyDistrHigh);
	std::uniform_int_distribution<> valuesDist(valuesDistrLow, valuesDistrHigh);
	std::cout << "Keys to update uniformly drawn from range [" << keyDistrLow << ", " << keyDistrHigh << "]\n";
	std::cout << "Updates performed with " <<  valuesDistrLow << "-" << valuesDistrHigh << " values\n";
	std::cout << "Updating...\n";
	std::vector<std::vector<std::future<bool>>> updateFutures;
	updateFutures.reserve(op);
	std::chrono::duration<double, std::milli> ms_double(0);
	std::chrono::steady_clock::time_point t1;
	std::chrono::steady_clock::time_point t2;
	for (int i = 0; i < op; i++) {
		int k = keyDist(gen);
		std::vector<int> v;
		for (int j = 0; j < valuesDist(gen); j++) {
			v.push_back(keyDist(gen));
		}
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
	int misses = op - hits;
	std::cout << "Update finished in: " << ms_double.count() << " ms\n";
	std::cout << "Successful updates: " << hits << "\n";
	std::cout << "Unsuccessful updates (key not present in tree): " << misses << "\n";
	std::cout << "Update performance: " << op / (ms_double.count() / 1000) << " ops\n";
}

void Program::updateOrInsertTest(const int op) {
	std::cout << "---Update or insert performance test---\n";
	int keyDistrLow = 1;
	int keyDistrHigh = 500000;
	int valuesDistrLow = 1;
	int valuesDistrHigh = 5;
	buildRandomTree(1000000, keyDistrLow, keyDistrHigh);
	std::cout << "Update operations to perform: " << op << "\n";
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> keyDist(keyDistrLow, keyDistrHigh);
	std::uniform_int_distribution<> valuesDist(valuesDistrLow, valuesDistrHigh);
	std::cout << "Keys to update uniformly drawn from range [" << keyDistrLow << ", " << keyDistrHigh << "]\n";
	std::cout << "Updates performed with " <<  valuesDistrLow << "-" << valuesDistrHigh << " values\n";
	std::cout << "Retrieving number of keys in each sub Bplustree for later use...\n";
	std::vector<int> beforeTreeNumKeys = pbtree->getTreeNumKeys();
	std::cout << "Updating...\n";
	std::vector<std::vector<std::future<bool>>> updateFutures;
	updateFutures.reserve(op);
	std::chrono::duration<double, std::milli> ms_double(0);
	std::chrono::steady_clock::time_point t1;
	std::chrono::steady_clock::time_point t2;
	for (int i = 0; i < op; i++) {
		int k = keyDist(gen);
		std::vector<int> v;
		for (int j = 0; j < valuesDist(gen); j++) {
			v.push_back(keyDist(gen));
		}
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
	std::cout << "Update or insert performance: " << op / (ms_double.count() / 1000) << " ops\n";
}
