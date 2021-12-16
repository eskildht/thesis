#include "bplustree.hpp"

Bplustree::Bplustree(int order) : order(order)
{
    root = new LeafNode();
}

int Bplustree::getOrder()
{
    return order;
}