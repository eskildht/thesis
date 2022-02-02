#include "program.hpp"
#include <iostream>
#include <sstream>

void printHelpInfo() {
	std::cout << "USAGE:\n";
	std::cout << "\t./optimized [OPTIONS]\n";
	std::cout << "\n";
	std::cout << "FLAGS:\n";
	std::cout << "\t--bloom-disable             " << "Disable bloom filter usage if --tree option has value parallel\n";
	std::cout << "\t--help                      " << "Print this help information\n";
	std::cout << "\n";
	std::cout << "OPTIONS:\n";
	std::cout << "\t--build-distr-high <num>    " << "Highest possible key value during tree build [default: 1000000]\n";
	std::cout << "\t--build-distr-low <num>     " << "Lowest possible key value during tree build [default: 1]\n";
	std::cout << "\t--op <num>      " << "Number of operations to perform for the --test value specified [default: 1000000]\n";
	std::cout << "\t--op-distr-high <num>       " << "Highest possible key value during test operation [default: 1000000]\n";
	std::cout << "\t--op-distr-low <num>        " << "Lowest possible key value during test operation [default: 1]\n";
	std::cout << "\t--order <num>               " << "Order of the Bplustree(s) [default: 5]\n";
	std::cout << "\t--test <test>               " << "The test to carry out [default: ] [possible values: delete, insert, search, update, updateorinsert]\n";
	std::cout << "\t--threads <num>             " << "Number of threads to use in the thread pool if --tree has value parallel [default: std::thread::hardware_concurrency()]\n";
	std::cout << "\t--tree <type>               " << "The tree data structure to create [default: parallel] [possible values: basic, parallel]\n";
	std::cout << "\t--trees <num>               " << "Number of Bplustrees to use if --tree option has value parallel [default: std::thread::hardware_concurrency()]\n";
}

std::tuple<std::map<std::string, bool>, std::tuple<std::map<std::string, int>, std::map<std::string, std::string>>> parseUserInput(int argc, char *argv[]) {
	std::map<std::string, bool> flagsBool = {
		{"--bloom-disable", false},
	 	{"--help", false}
	};
	std::map<std::string, int> optionsInt = {
		{"--build-distr-high", 1000000},
		{"--build-distr-low", 1},
		{"--op", 1000000},
		{"--op-distr-high", 1000000},
		{"--op-distr-low", 1},
		{"--order", 5},
		{"--threads", std::thread::hardware_concurrency()},
		{"--trees", std::thread::hardware_concurrency()}
	};
	std::map<std::string, std::string> optionsString = {
		{"--test", ""},
		{"--tree", "parallel"}
	};
	std::map<std::string, std::vector<std::string>> optionsStringPossibleValues = {
		{"--test", {"delete", "insert", "search", "update", "updateorinsert"}},
		{"--tree", {"basic", "parallel"}}
	};
	try {
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
					msg << argv[i+1] << " is not a possible value for " << argv[i];
					throw msg.str();
				}
			}
		}

	}
	catch (std::basic_stringstream<char>::string_type errorMsg) {
		std::cout << errorMsg;
	}
	catch (...) {
		std::cout << "Error occured during parseUserInput: check flags and options\n";
	}
	return std::make_tuple(flagsBool, std::make_tuple(optionsInt, optionsString));
}

int main(int argc, char *argv[]) {
	
}
