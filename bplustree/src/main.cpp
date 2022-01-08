#include "bplustree.hpp"
#include <random>
#include <iostream>

void createDeterministicTree(Bplustree *tree) {
	std::vector<int> keys({55, 45, 40, 35, 30, 25, 20, 15, 5});
	std::vector<int> values({1, 2, 3, 4, 5, 6, 7, 8, 9});
	for (int i=0; i < keys.size(); i++) {
		tree->insert(keys[i], values[i]);
	}
}

void createRandomTree(Bplustree *tree, int nInserts, int distLow, int distUpper) {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(distLow, distUpper);

	for(int n=0; n<nInserts; n++) {
		int k = distr(gen);
		int v = distr(gen);
		tree->insert(k, v);
	}
}

void updateTest() {
	std::cout << "---Update Test---\n";
	Bplustree tree(4);
	createDeterministicTree(&tree);
	tree.show();
	int k = 17;
	const std::vector<int> *values = tree.search(k);
	if (values) {
		std::cout << "Key=" << k << ": ";
		for (int val : *values) {
			std::cout << val << " ";
		}
	}
	else {
		std::cout << "Key=" << k << ": Not found";
	}
	std::cout << std::endl;
	std::vector<int> updateValues{1, 2, 3};
	tree.update(k, updateValues);
	values = tree.search(k);
	if (values) {
		std::cout << "Key=" << k << ": ";
		for (int val : *values) {
			std::cout << val << " ";
		}
	}
	else {
		std::cout << "Key=" << k << ": Not found";
	}
}

void searchTest() {
	std::cout << "---Search Test---\n";
	int nSearches = 20;
	Bplustree tree(4);
	int distLow = 1;
	int distUpper = 250;
	createRandomTree(&tree, 250, distLow, distUpper);
	tree.show();
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(distLow, distUpper);
	for(int n=0; n<nSearches; n++) {
		int k = distr(gen);
		const std::vector<int> *values = tree.search(k);
		if (values) {
			std::cout << "Key=" << k << ": ";
			for (int val : *values) {
				std::cout << val << " ";
			}
		}
		else {
			std::cout << "Key=" << k << ": Not found";
		}
		std::cout << std::endl;
	}
}

void scanTest() {
	std::cout << "---Scan Test---\n";
	Bplustree tree(4);
	createDeterministicTree(&tree);
	tree.show();
	std::cout  << "Scan 12, 37\n";
	std::map<int, std::vector<int>> result = tree.scan(12, 37);
	for (const auto &[key, values] : result) {
		std::cout << key << "=";
		for (const int &value : values) {
			std::cout << value << ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	std::cout  << "Scan -10, 19\n";
	result = tree.scan(-10, 19);
	for (const auto &[key, values] : result) {
		std::cout << key << "=";
		for (const int &value : values) {
			std::cout << value << ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	std::cout  << "Scan 0, 1901\n";
	result = tree.scan(0, 1901);
	for (const auto &[key, values] : result) {
		std::cout << key << "=";
		for (const int &value : values) {
			std::cout << value << ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	std::cout  << "Scan 93, 121\n";
	result = tree.scan(93, 121);
	for (const auto &[key, values] : result) {
		std::cout << key << "=";
		for (const int &value : values) {
			std::cout << value << ", ";
		}
		std::cout << "\n";
	}
}

void scanFullTest() {
	std::cout << "---Scan full Test---\n";
	Bplustree tree(4);
	createDeterministicTree(&tree);
	tree.show();
	std::map<int, std::vector<int>> result = tree.scanFull();
	for (const auto &[key, values] : result) {
		std::cout << key << "=";
		for (const int &value : values) {
			std::cout << value << ", ";
		}
		std::cout << "\n";
	}
}

void deleteTest() {
	Bplustree tree(3);
	createDeterministicTree(&tree);
	tree.show();
}

int main() {
	//searchTest();
	//std::cout << std::endl;
	//updateTest();
	//std::cout << std::endl;
	//scanTest();
	//std::cout << std::endl;
	//scanFullTest();
	deleteTest();
}

