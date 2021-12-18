#include "bplustree.hpp"

int main()
{
    Bplustree tree(3);
    tree.insert(7, 107);
    tree.insert(8, 108);
    tree.insert(9, 109);
    tree.insert(11, 111);
    tree.show();
    // int keys[8] = {7, 12, 19, 21, 5, 20, 17, 15};
    // int values[8] = {107, 112, 119, 121, 105, 117, 115};
    // for (int k : keys) {
    //     for (int v : values) {
    //         tree.insert(k, v);
    //     }
    // }
}