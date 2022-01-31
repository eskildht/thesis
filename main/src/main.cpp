#include "program.hpp"
#include <iostream>

void printHelpInfo() {
	std::cout << "Usage: ./optimized (./debug) [options]\n";
	std::cout << "Available options:\n";
	std::cout << "--order    " << "Order of the Bplustrees (default 5)\n";
	std::cout << "--threads  " << "Number of threads to use in the thread pool (default std::thread::hardware_concurrency())\n";
	std::cout << "--trees    " << "Number of base Bplustrees (default std::thread::hardware_concurrency())\n";
	std::cout << "--bloom    " << "Enable or disable bloom filter usage (default 1)\n";
	std::cout << "--test     " << "The test to perform (default \"\"), MUST be passed with one of the following: insert, search, delete, update, updateorinsert\n";
	std::cout << "--op       " << "Number of operations to perform for the test specified (default 10000)\n";
	std::cout << "--help     " << "Print this help\n";
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
	if (test == "") {
		std::cout << "Test to run was not specified. Pass --test with one of the following:\ninsert, search, update, updateorinsert\n";
		return 0;
	}
	Program mainPB(order, threads, trees, bloom);
	mainPB.printTreeInfo();
	if (test == "insert") {
		mainPB.insertTest(op);
	}
	else if (test == "search") {
		mainPB.searchTest(op);
	}
	else if (test == "delete") {
		mainPB.deleteTest(op);
	}
	else if (test == "update") {
		mainPB.updateTest(op);
	}
	else if (test == "updateorinsert") {
		mainPB.updateOrInsertTest(op);
	}
}
