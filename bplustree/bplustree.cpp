#include "bplustree.cpp"

Bplustree::Bplustree(int order) order(order)
{
    LeafNode node = new LeafNode();
    root = &node;
}

Bplustree::getOrder()
{
    return order;
}