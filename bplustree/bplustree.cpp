#include "bplustree.hpp"

Bplustree::Bplustree(int order) : order(order)
{
    root = new LeafNode();
}

int Bplustree::getOrder()
{
    return order;
}

void Bplustree::insert(int key, int value) {
    std::stack<Node *> path;
    findSearchPath(key, &path);
    Node *leaf = path.top();
    path.pop();
    leaf->insert(key, value);
    if (leaf->getKeys()->size() == order) {
        Node *right = leaf->split();
        int keyToParent = (*(right->getKeys()))[0];
        Node *internal = path.top();
        path.pop();
        internal->insert(keyToParent, right);
        while(internal->getKeys()->size() == order) {
            right = internal->split();
            keyToParent = (*(right->getKeys()))[0];
            if (path.size() > 0) {
                internal = path.top();
                path.pop();
            }
            else {
                Node *newRoot = new InternalNode();
                newRoot->insert(keyToParent, internal, right);
                root = newRoot;
            }
            internal->insert(keyToParent, right);
        }
    }
    else {
        return;
    }
    
}

void Bplustree::findSearchPath(int key, std::stack<Node *> *path) {
    path->push(root);
    if (root->isLeaf()) {
        return;
    }
}

void Bplustree::show() {
    printTree(root, 0, "");
}

void Bplustree::printTree(Node *node, int printLevel, std::string prevString) {
    std::string baseString = prevString;
    prevString = printNode(root, printLevel, baseString);
    if (!node->isLeaf()) {
        InternalNode *internal = static_cast<InternalNode*>(node);
        for (int i = internal->getChildren()->size(); i >= 0; i--) {
            printTree((*(internal->getChildren()))[i], i, prevString);
        }
    }
}

std::string Bplustree::printNode(Node *node, int printLevel, std::string prevString) {
    prevString += "     ";
    for (int i = 1; i <= printLevel; i++) {
        prevString += "|  ";
    }
    std::cout << prevString << " `---.";
    for (int i = 0; i < node->getKeys()->size(); i++) {
        std::cout << (*(node->getKeys()))[i] << ".";
    }
    std::cout << std::endl;
    return prevString;
}