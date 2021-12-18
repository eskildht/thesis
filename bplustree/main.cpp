#include "bplustree.hpp"

int main()
{
    Bplustree tree(4);
    int keys[14] = {7, 12, 19, 21, 5, 20, 17, 15, 91, 21, 111, 142, 7, 16};
    int values[14] = {107, 112, 119, 121, 105, 117, 115, 122, 12311, 12, 12, 192, 23, 182};
    for (int k : keys)
    {
        for (int v : values)
        {
            tree.insert(k, v);
        }
    }
    tree.show();
}