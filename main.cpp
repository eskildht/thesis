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

void insertTest(const int op, ParallelBplustree *tree) {
	std::cout << "---Insert performance test---\n";
	std::random_device rd;
	std::mt19937_64 gen(rd());
	int distLower = 1;
	int distUpper = 1000000;
	std::uniform_int_distribution<> distr(1, 1000000);
	std::cout << "Insert operations to perform: " << op << "\n";
	std::cout << "Key/Value pairs uniformly drawn from range [" << distLower << ", " << distUpper << "]\n";
	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < op; i++) {
		int k = distr(gen);
		int v = distr(gen);
		tree->insert(k, v);
	}
	tree->waitForWorkToFinish();
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	std::cout << ms_double.count() << " ms spent on insert operation\n";
	std::cout << "Performance: " << op / (ms_double.count() / 1000) << " ops\n";
}

int main(int argc, char *argv[]) {
	std::vector<std::string> validArgv = {"--order", "--threads", "--trees", "--bloom", "--test", "--op"};
	// Valif default values for all args except --test
	int order = 5;
	int threads = std::thread::hardware_concurrency();
	int trees = std::thread::hardware_concurrency();
	bool bloom = true;
	std::string test = "";
	int op = 10000;
	// Parse argv and set arguments
	for (int i = 1; i < argc - 1; i++) {
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
			}
		}
	}
	ParallelBplustree tree(order, threads, trees, bloom);
	printTreeInfo(order, threads, trees, bloom);
	if (test == "") {
		std::cout << "Test to run was not specified. Pass --test with one of the following:\ninsert\n";
		return 0;
	}
	else if (test == "insert") {
		insertTest(op, &tree);
	}
}
