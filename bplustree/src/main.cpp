#include "bplustree.hpp"
#include <random>
#include <iostream>

void createDeterministicTree(Bplustree *tree) {
	std::vector<int> keys({2, 3, 5, 7, 14, 16, 19, 20, 22, 24, 27, 29, 34, 38, 39, 6, 8, 9, 10});
	std::vector<int> values({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20});
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
	std::cout << "---Delete Test---\n";
	Bplustree tree(5);
	createDeterministicTree(&tree);
	std::cout << "Initial tree\n";
	tree.show();
	std::cout << "Test simple delete: Deleting 34\n";
	tree.remove(34);
	tree.show();
	std::cout << "Test leaf redistribute: Deleting 29\n";
	tree.remove(29);
	tree.show();
	std::cout << "Test leaf merge: Deleting 24\n";
	tree.remove(24);
	tree.show();
	std::cout << "Test internal redistribute: Deleting 19\n";
	tree.remove(19);
	tree.show();
	std::cout << "Test internal merge: Deleting 3\n";
	tree.remove(3);
	tree.show();
	std::cout << "Test delete of non-existing key: Deleting 4000\n";
	tree.remove(4000);
	tree.show();
	std::cout << "Test delete of all keys in tree: Deleting {2, 5, 6, 7, 8, 9, 10, 14, 16, 20, 22, 27, 38, 39}\n";
	std::vector<int> remainingKeys({2, 5, 6, 7, 8, 9, 10, 14, 16, 20, 22, 27, 38, 39});
	for (int i = 0; i < remainingKeys.size(); i++) {
		tree.remove(remainingKeys[i]);
	}
	tree.show();
	std::cout << "Test delete when tree is empty: Deleting 129\n";
	tree.remove(129);
	tree.show();
	std::cout << "---Delete Test Finished---" << std::endl;
}

int main() {
	searchTest();
	std::cout << std::endl;
	updateTest();
	std::cout << std::endl;
	scanTest();
	std::cout << std::endl;
	scanFullTest();
	deleteTest();
}

