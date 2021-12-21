#include "bplustree.hpp"
#include <random>

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

void createRandomTree(Bplustree *tree, int nInserts) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<> distr(1, 250);

    for(int n=0; n<nInserts; n++) {
        int k = distr(gen);
        int v = distr(gen);
        tree->insert(k, v);
    }
}

int main()
{
    Bplustree tree(4);
    createRandomTree(&tree, 100);
    tree.show();
}
