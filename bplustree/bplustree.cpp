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
    findSearchPath(key, root, &path);
    Node *leaf = path.top();
    path.pop();
    leaf->insert(key, value);
    if (leaf->getKeys()->size() == order) {
        Node *right = leaf->split();
        int keyToParent = (*(right->getKeys()))[0];
        if (path.empty()) {
            Node *newRoot = new InternalNode();
            newRoot->insert(keyToParent, leaf, right);
            root = newRoot;
            return;
        }
        Node *internal = path.top();
        path.pop();
        internal->insert(keyToParent, right);
        while(internal->getKeys()->size() == order) {
            right = internal->split();
            keyToParent = (*(right->getKeys()))[0];
            if (!path.empty()) {
                internal = path.top();
                path.pop();
                internal->insert(keyToParent, right);
            }
            else {
                Node *newRoot = new InternalNode();
                newRoot->insert(keyToParent, internal, right);
                root = newRoot;
                return;
            }
        }
    }
    else {
        return;
    }
    
}

void Bplustree::findSearchPath(int key, Node* node, std::stack<Node *> *path) {
    path->push(node);
    if (node->isLeaf()) {
        return;
    }
    std::vector<int> *keys = node->getKeys();
	std::vector<int>::iterator low = std::lower_bound(keys->begin(), keys->end(), key);
    InternalNode *internal = static_cast<InternalNode*>(node);
    Node *nextNode = nullptr;
    if (key == (*keys)[low - keys->begin()]) {
        nextNode = (*(internal->getChildren()))[low - keys->begin() + 1];
    }
    else {
        nextNode = (*(internal->getChildren()))[low - keys->begin()];
    }
    findSearchPath(key, nextNode, path);
}

void Bplustree::show() {
    printTree(root, 0, "");
}

void Bplustree::printTree(Node *node, int printLevel, std::string prevString) {
    std::string baseString = prevString;
    prevString = printNode(node, printLevel, baseString);
    if (!node->isLeaf()) {
        InternalNode *internal = static_cast<InternalNode*>(node);
        for (int i = internal->getChildren()->size() - 1; i >= 0; i--) {
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