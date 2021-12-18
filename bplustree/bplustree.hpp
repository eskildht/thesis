#include "node.hpp"
#include <stack>

class Bplustree
{
public:
    Bplustree(int order);
    int getOrder();
    void insert(int key, int value);

private:
    int order;
    Node *root;
    void findSearchPath(int key, std::stack<Node *> *path);
};