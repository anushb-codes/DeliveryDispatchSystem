#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include "Entities.h"
#include <iostream>
#include <string>
#include <vector>

class HeapNode {
public:
    Order* data;
    int degree;
    HeapNode* parent;
    HeapNode* child;
    HeapNode* sibling;

    HeapNode(Order* order) {
        data = order;
        degree = 0;
        parent = nullptr;
        child = nullptr;
        sibling = nullptr;
    }
};

class BinomialHeap {
private:
    HeapNode* head;

    void link(HeapNode* y, HeapNode* z);
    HeapNode* mergeRoots(HeapNode* h1, HeapNode* h2);
    HeapNode* unionHeaps(HeapNode* h1, HeapNode* h2);
    void destroyTree(HeapNode* node);
    void printTree(HeapNode* node, std::string prefix, bool isTail);
    
    // Newly added declaration
    void collectOrders(HeapNode* node, std::vector<Order*>& list);

public:
    BinomialHeap();
    ~BinomialHeap();

    void insert(Order* order);
    Order* extractMin();
    bool isEmpty();
    void printHeap();
    
    // Newly added declaration
    std::vector<Order*> getPendingOrders();
};

#endif