#include "program.hpp"
#include <iostream>
#include <sstream>

void printHelpInfo() {
	std::cout << "USAGE:\n";
	std::cout << "\t./optimized [FLAGS] [OPTIONS] --test <test>\n";
	std::cout << "\n";
	std::cout << "FLAGS:\n";
	std::cout << "\t--batch                     " << "Enable batch insert during --test insert and general build, if --tree option has value parallel\n";
	std::cout << "\t--bloom-disable             " << "Disable bloom filter usage if --tree option has value parallel\n";
	std::cout << "\t--help                      " << "Print this help information\n";
	std::cout << "\t--show                      " << "Print the tree after build if --tree-size value <= 1000\n";
	std::cout << "\n";
	std::cout << "OPTIONS:\n";
	std::cout << "\t--build-distr-high <num>    " << "Highest possible key value during tree build [default: 1000000]\n";
	std::cout << "\t--build-distr-low <num>     " << "Lowest possible key value during tree build [default: 1]\n";
	std::cout << "\t--op <num>                  " << "Number of operations to perform for the --test value specified [default: 1000000]\n";
	std::cout << "\t--op-distr-high <num>       " << "Highest possible key value during test operation [default: 1000000]\n";
	std::cout << "\t--op-distr-low <num>        " << "Lowest possible key value during test operation [default: 1]\n";
	std::cout << "\t--order <num>               " << "Order of the Bplustree(s) [default: 5]\n";
	std::cout << "\t--test <test>               " << "The test to carry out [default: ] [possible values: delete, insert, search, update]\n";
	std::cout << "\t--threads <num>             " << "Number of threads to use in the thread pool if --tree has value parallel [default: std::thread::hardware_concurrency()]\n";
	std::cout << "\t--tree <type>               " << "The tree data structure to create [default: parallel] [possible values: basic, parallel]\n";
	std::cout << "\t--tree-size <num>           " << "The number of inserts to do during tree build (overridden by --op if --test has value insert) [default: 1000000]\n";
	std::cout << "\t--trees <num>               " << "Number of Bplustrees to use if --tree option has value parallel [default: std::thread::hardware_concurrency()]\n";
}

std::tuple<std::map<std::string, bool>, std::map<std::string, int>, std::map<std::string, std::string>> parseUserInput(int argc, char *argv[]) {
	std::map<std::string, bool> flagsBool = {
		{"--batch", false},
		{"--bloom-disable", false},
		{"--help", false},
		{"--show", false}
	};
	std::map<std::string, int> optionsInt = {
		{"--build-distr-high", 1000000},
		{"--build-distr-low", 1},
		{"--op", 1000000},
		{"--op-distr-high", 1000000},
		{"--op-distr-low", 1},
		{"--order", 5},
		{"--threads", std::thread::hardware_concurrency()},
		{"--trees", std::thread::hardware_concurrency()},
		{"--tree-size", 1000000}
	};
	std::map<std::string, std::string> optionsString = {
		{"--test", ""},
		{"--tree", "parallel"}
	};
	std::map<std::string, std::vector<std::string>> optionsStringPossibleValues = {
		{"--test", {"delete", "insert", "search", "update"}},
		{"--tree", {"basic", "parallel"}}
	};
	for (int i = 1; i < argc; i++) {
		if (flagsBool.count(argv[i])) {
			flagsBool[argv[i]] = true;
		}
		else if (optionsInt.count(argv[i])) {
			optionsInt[argv[i]] = std::stoi(argv[i+1]);
		}
		else if (optionsString.count(argv[i])) {
			std::vector<std::string> &possibleValues = optionsStringPossibleValues[argv[i]];
			if (std::find(possibleValues.begin(), possibleValues.end(), argv[i+1]) != possibleValues.end()) {
				optionsString[argv[i]] = argv[i+1];
			}
			else {
				std::stringstream msg;
				msg << argv[i+1] << " is not a possible value for " << argv[i] << "\n";
				throw msg.str();
			}
		}
	}
	if (optionsString["--test"] == "" && !flagsBool["--help"]) {
		throw std::string("Test to run must be specified using --test option [possible values: delete, insert, search, update]\n");
	}
	return std::make_tuple(flagsBool, optionsInt, optionsString);
}

int main(int argc, char *argv[]) {
	std::tuple<std::map<std::string, bool>, std::map<std::string, int>, std::map<std::string, std::string>> result;
	try {
		result = parseUserInput(argc, argv);
	}
	catch (std::string errorMsg) {
		std::cout << errorMsg;
		return 0;
	}
	catch (...) {
		std::cout << "Error occured during parseUserInput: check flags and options\n";
		return 0;
	}
	std::map<std::string, bool> &flagsBool = std::get<0>(result);
	std::map<std::string, int> &optionsInt = std::get<1>(result);
	std::map<std::string, std::string> &optionsString = std::get<2>(result);
	if (flagsBool["--help"]) {
		printHelpInfo();
		return 0;
	}
	else {
		Program program(optionsString["--tree"], optionsInt["--order"], optionsInt["--threads"], optionsInt["--trees"], !flagsBool["--bloom-disable"], optionsInt["--op"], optionsInt["--op-distr-low"], optionsInt["--op-distr-high"], optionsInt["--build-distr-low"], optionsInt["--build-distr-high"], flagsBool["--show"], flagsBool["--batch"], optionsInt["--tree-size"], optionsString["--test"]);
		program.runTest();
	}
}
