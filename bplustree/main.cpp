#include <iostream>
#include "node.hpp"

int main()
{
    InternalNode in1;
    LeafNode ln1;
    in1.insert(10);
    ln1.insert(10, 90);
    ln1.insert(10, 120);
    ln1.insert(20, 120);

    for (auto key : *in1.getKeys())
    {
        std::cout << key << " ";
    }
    std::cout << "\n------\n";
    for (auto key : *ln1.getKeys())
    {
        std::cout << key << "\n";
        std::cout << "\t";
        for (auto value : *ln1.getValues(key))
        {
            std::cout << value << " ";
        }
        std::cout << "\n";
    }
}