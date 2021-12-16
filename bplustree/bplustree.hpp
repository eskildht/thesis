#include "node.hpp"

class Bplustree
{
public:
    Bplustree(int order);
    int getOrder();

private:
    int order;
    Node *root;
};