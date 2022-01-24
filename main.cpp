#include "parallelbplustree.hpp"
#include <random>
#include <iostream>

void printTreeInfo(const int order, const int threads, const int trees, const bool bloom) {
	std::cout << "---ParallelBplustree information--\n";
	std::cout << "order: " << order << "\n";
	std::cout << "threads: " << threads << "\n";
	std::cout << "trees: " << trees << "\n";
	std::cout << "bloom: " << bloom << "\n";
}

void printHelpInfo() {
	std::cout << "Usage: ./optimized (./debug) [options]\n";
	std::cout << "Available options:\n";
	std::cout << "--order    " << "Order of the Bplustrees (default 5)\n";
	std::cout << "--threads  " << "Number of threads to use in the thread pool (default std::thread::hardware_concurrency())\n";
	std::cout << "--trees    " << "Number of base Bplustrees (default std::thread::hardware_concurrency())\n";
	std::cout << "--bloom    " << "Enable or disable bloom filter usage (default 1)\n";
	std::cout << "--test     " << "The test to perform (default \"\"), MUST be passed with one of the following: insert, search, delete\n";
	std::cout << "--op       " << "Number of operations to perform for the test specified (default 10000)\n";
	std::cout << "--help     " << "Print this help\n";
}

void buildRandomTree(ParallelBplustree *tree, const int numInserts, const int distLower, const int distUpper) {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(distLower, distUpper);
	std::cout << "Tree to be built by " << numInserts << " inserts\n";
	std::cout << "Key/value pairs uniformly drawn from range [" << distLower << ", " << distUpper << "]\n";
	std::cout << "Building...\n";
	std::vector<std::future<void>> buildFutures;
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < numInserts; i++) {
		int k = distr(gen);
		int v = distr(gen);
		buildFutures.push_back(std::move(tree->insert(k, v)));
	}
	for(int i = 0; i < numInserts; i++) {
		buildFutures[i].wait();
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Build finished in: " << ms_double.count() << " ms\n";
	std::cout << "Build performance: " << numInserts / (ms_double.count() / 1000) << " ops\n";
}

void searchTest(const int op, ParallelBplustree *tree) {
	std::cout << "---Search performance test---\n";
	int distLower = 1;
	int distUpper = 500000;
	buildRandomTree(tree, 1000000, distLower, distUpper);
	std::cout << "Search operations to perform: " << op << "\n";
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(distLower, distUpper);
	std::cout << "Keys to search for uniformly drawn from range [" << distLower << ", " << distUpper << "]\n";
	std::cout << "Searching...\n";
	std::vector<std::vector<std::future<const std::vector<int> *>>> searchFutures;
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < op; i++) {
		int k = distr(gen);
		searchFutures.push_back(std::move(tree->search(k)));
	}
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < searchFutures[i].size(); j++) {
			searchFutures[i][j].wait();
		}
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Search finished in: " << ms_double.count() << " ms\n";
	std::cout << "Search performance: " << op / (ms_double.count() / 1000) << " ops\n";
}

void deleteTest(const int op, ParallelBplustree *tree) {
	std::cout << "---Delete performance test---\n";
}

void insertTest(const int op, ParallelBplustree *tree) {
	std::cout << "---Insert performance test---\n";
	int distLower = 1;
	int distUpper = 500000;
	buildRandomTree(tree, op, distLower, distUpper);
}

int main(int argc, char *argv[]) {
	std::vector<std::string> validArgv = {"--order", "--threads", "--trees", "--bloom", "--test", "--op", "--help"};
	// Valid default values for all args except --test
	int order = 5;
	int threads = std::thread::hardware_concurrency();
	int trees = std::thread::hardware_concurrency();
	bool bloom = true;
	std::string test = "";
	int op = 10000;
	// If no arguments are passed print --help since at least --test is needed
	if (argc == 1) {
		printHelpInfo();
		return 0;
	}
	// Parse argv and set arguments
	for (int i = 1; i < argc; i++) {
		std::vector<std::string>::iterator it = std::find(validArgv.begin(), validArgv.end(), argv[i]);
		if (it != validArgv.end()) {
			switch (it - validArgv.begin()) {
				// order
				case 0:
					order = std::stoi(argv[i+1]);
					break;
				// threads
				case 1:
					threads = std::stoi(argv[i+1]);
					break;
				// trees
				case 2:
					threads = std::stoi(argv[i+1]);
					break;
				// bloom
				case 3:
					bloom = std::stoi(argv[i+1]) == 0 ? false : true;
					break;
				// test
				case 4:
					test = argv[i+1];
					break;
				// op
				case 5:
					op = std::stoi(argv[i+1]);
					break;
				case 6:
					printHelpInfo();
					return 0;
			}
		}
	}
	ParallelBplustree tree(order, threads, trees, bloom);
	if (test == "") {
		std::cout << "Test to run was not specified. Pass --test with one of the following:\ninsert, search\n";
		return 0;
	}
	printTreeInfo(order, threads, trees, bloom);
	if (test == "insert") {
		insertTest(op, &tree);
	}
	else if (test == "search") {
		searchTest(op, &tree);
	}
	else if (test == "delete") {
		deleteTest(op, &tree);
	}
}
