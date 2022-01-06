#include "bplustree.hpp"
#include <random>
#include <iostream>

void createDeterministicTree(Bplustree *tree) {
	std::vector<int> keys({7, 12, 19, 21, 5, 20, 17, 15, 91, 21, 111, 142, 7, 16});
	std::vector<int> values({107, 112, 119, 121, 105, 117, 115, 122, 12311, 12, 12, 192, 23, 182});
	for (int i=0; i < keys.size(); i++) {
		std::cout << "key=" << keys[i] << " value=" << values[i] << "\n";
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
	std::vector<int> *values = tree.search(k);
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
		std::vector<int> *values = tree.search(k);
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
	tree.insert(12, 999);
	std::map<int, std::vector<int>> result = tree.scan(12, 37);
	for (const auto &[key, values] : result) {
		std::cout << key << "=";
		for (const int &value : values) {
			std::cout << value << ", ";
		}
		std::cout << "\n";
	}
	std::cout << (*(tree.search(12)))[0];
	std::cout << (*(tree.search(12)))[1];
}

int main() {
	searchTest();
	std::cout << std::endl;
	updateTest();
	std::cout << std::endl;
	scanTest();
}

