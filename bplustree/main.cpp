#include <iostream>
#include "node.hpp"

int main()
{
    LeafNode ln1;
    ln1.insert(10, 90);
    ln1.insert(10, 120);
    ln1.insert(20, 120);

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