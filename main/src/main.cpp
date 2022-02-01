#include "program.hpp"
#include <iostream>

void printHelpInfo() {
	std::cout << "USAGE:\n";
	std::cout << "\t./optimized [OPTIONS]\n";
	std::cout << "\n";
	std::cout << "FLAGS:\n";
	std::cout << "\t--bloom-disable             " << "Disable bloom filter usage if --tree option has value parallel\n";
	std::cout << "\t--help                      " << "Print this help information\n";
	std::cout << "\n";
	std::cout << "OPTIONS:\n";
	std::cout << "\t--build-distr-low <num>     " << "Lowest possible key value during tree build [default: 1]\n";
	std::cout << "\t--build-distr-high <num>    " << "Highest possible key value during tree build [default: 1000000]\n";
	std::cout << "\t--op <num>      " << "Number of operations to perform for the --test value specified [default: 1000000]\n";
	std::cout << "\t--op-distr-low <num>        " << "Lowest possible key value during test operation [default: 1]\n";
	std::cout << "\t--op-distr-high <num>       " << "Highest possible key value during test operation [default: 1000000]\n";
	std::cout << "\t--order <num>               " << "Order of the Bplustree(s) [default: 5]\n";
	std::cout << "\t--test <test>               " << "The test to carry out [default: ] [possible values: delete, insert, search, update, updateorinsert]\n";
	std::cout << "\t--threads <num>             " << "Number of threads to use in the thread pool if --tree has value parallel [default: std::thread::hardware_concurrency()]\n";
	std::cout << "\t--tree <type>               " << "The tree data structure to create [default: parallel] [possible values: basic, parallel]\n";
	std::cout << "\t--trees <num>               " << "Number of Bplustrees to use if --tree option has value parallel [default: std::thread::hardware_concurrency()]\n";
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
	Program program(order, threads, trees, bloom);
	program.printTreeInfo();
	if (test == "insert") {
		program.insertTest(op);
	}
	else if (test == "search") {
		program.searchTest(op);
	}
	else if (test == "delete") {
		program.deleteTest(op);
	}
	else if (test == "update") {
		program.updateTest(op);
	}
	else if (test == "updateorinsert") {
		program.updateOrInsertTest(op);
	}
}
