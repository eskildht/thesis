#include "bplustree.hpp"
#include <random>
#include <iostream>

void createDeterministicTree(Bplustree *tree) {
    int keys[14] = {7, 12, 19, 21, 5, 20, 17, 15, 91, 21, 111, 142, 7, 16};
    int values[14] = {107, 112, 119, 121, 105, 117, 115, 122, 12311, 12, 12, 192, 23, 182};
    for (int k : keys)
    {
        for (int v : values)
        {
            tree->insert(k, v);
        }
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

void searchTest(Bplustree *tree, int nSearches, int distLow, int distUpper) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<> distr(distLow, distUpper);

    for(int n=0; n<nSearches; n++) {
        int k = distr(gen);
				std::vector<int> *values = tree->search(k);
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

int main()
{
    Bplustree tree(4);
		int distLow = 1;
		int distUpper = 250;
    createRandomTree(&tree, 250, distLow, distUpper);
    tree.show();
		searchTest(&tree, 15, distLow, distUpper);
}
