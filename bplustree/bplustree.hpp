#include "node.hpp"
#include <iostream>
#include <stack>
#include <string>

class Bplustree
{
public:
    Bplustree(int order);
    int getOrder();
    void insert(int key, int value);
    void show();

private:
    int order;
    Node *root;
    void findSearchPath(int key, Node* node, std::stack<Node *> *path);
    void printTree(Node *node, int level, std::string prevString, std::vector<int> *parentKeyLengths);
    std::string printNode(Node *node, int level, std::string prevString, std::vector<int> *parentKeyLengths);
};