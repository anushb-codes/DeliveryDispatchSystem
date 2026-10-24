#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "Entities.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class AVLNode {
public:
    Order* data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Order* order) {
        data = order;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

class AVLTree {
private:
    AVLNode* root;

    void destroyTree(AVLNode* node);
    int getHeight(AVLNode* node);
    int getBalanceFactor(AVLNode* node);
    
    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);
    
    AVLNode* insertHelper(AVLNode* node, Order* order);
    AVLNode* getMinValueNode(AVLNode* node);
    AVLNode* deleteHelper(AVLNode* root, int orderID);
    
    AVLNode* searchHelper(AVLNode* root, int orderID);
    void printHelper(AVLNode* root, int space);
    
    // Newly added declaration
    void collectOrders(AVLNode* node, std::vector<Order*>& list);

public:
    AVLTree();
    ~AVLTree();

    void insert(Order* order);
    void remove(int orderID);
    Order* search(int orderID);
    void printTree();
    
    // Newly added declaration
    std::vector<Order*> getActiveDeliveries();
};

#endif