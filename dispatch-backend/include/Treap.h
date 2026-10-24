#ifndef TREAP_H
#define TREAP_H

#include "Entities.h"
#include <iostream>
#include <string>
#include <vector>

class TreapNode {
public:
    Driver* data;
    TreapNode* left;
    TreapNode* right;

    TreapNode(Driver* d) {
        data = d;
        left = nullptr;
        right = nullptr;
    }
};

class Treap {
private:
    TreapNode* root;

    void destroyTree(TreapNode* node);
    TreapNode* rightRotate(TreapNode* y);
    TreapNode* leftRotate(TreapNode* x);
    
    TreapNode* insertHelper(TreapNode* node, Driver* driver);
    TreapNode* deleteHelper(TreapNode* root, int driverID);
    void printHelper(TreapNode* root, int space);
    
    // Newly added declaration
    void collectDrivers(TreapNode* node, std::vector<Driver*>& list);

public:
    Treap();
    ~Treap();

    void insert(Driver* driver);
    void remove(int driverID);
    Driver* extractMostAvailable();
    bool isEmpty();
    void printTreap();
    
    // Newly added declaration
    std::vector<Driver*> getAvailableDrivers();
};

#endif