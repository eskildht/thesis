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

