#include "../include/Treap.h"
using namespace std;

Treap::Treap() {
    root = nullptr;
}

Treap::~Treap() {
    destroyTree(root);
}

void Treap::destroyTree(TreapNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node; // Deletes the node, but DispatchSystem manages the Driver* data
    }
}

void Treap::collectDrivers(TreapNode* node, std::vector<Driver*>& list) {
    if (node == nullptr) return;
    collectDrivers(node->left, list);
    list.push_back(node->data);
    collectDrivers(node->right, list);
}

std::vector<Driver*> Treap::getAvailableDrivers() {
    std::vector<Driver*> list;
    collectDrivers(root, list);
    return list;
}
TreapNode* Treap::rightRotate(TreapNode* y) {
    TreapNode* x = y->left;
    TreapNode* T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    return x;
}

TreapNode* Treap::leftRotate(TreapNode* x) {
    TreapNode* y = x->right;
    TreapNode* T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    return y;
}

TreapNode* Treap::insertHelper(TreapNode* node, Driver* driver) {
    if (node == nullptr) {
        return new TreapNode(driver);
    }

    if (driver->driverID <= node->data->driverID) {
        node->left = insertHelper(node->left, driver);
        
        // Max-Heap property based on availability score
        if (node->left->data->availabilityScore > node->data->availabilityScore) {
            node = rightRotate(node);
        }
    } else {
        node->right = insertHelper(node->right, driver);
        
        if (node->right->data->availabilityScore > node->data->availabilityScore) {
            node = leftRotate(node);
        }
    }
    return node;
}

void Treap::insert(Driver* driver) {
    root = insertHelper(root, driver);
}

TreapNode* Treap::deleteHelper(TreapNode* root, int driverID) {
    if (root == nullptr) {
        return root;
    }

    if (driverID < root->data->driverID) {
        root->left = deleteHelper(root->left, driverID);
    } else if (driverID > root->data->driverID) {
        root->right = deleteHelper(root->right, driverID);
    } else {
        // Node found
        if (root->left == nullptr) {
            TreapNode* temp = root->right;
            delete root;
            root = temp;
        } else if (root->right == nullptr) {
            TreapNode* temp = root->left;
            delete root;
            root = temp;
        } else {
            // Push down the node with the lower availability score to maintain Max-Heap
            if (root->left->data->availabilityScore > root->right->data->availabilityScore) {
                root = rightRotate(root);
                root->right = deleteHelper(root->right, driverID);
            } else {
                root = leftRotate(root);
                root->left = deleteHelper(root->left, driverID);
            }
        }
    }
    return root;
}

void Treap::remove(int driverID) {
    root = deleteHelper(root, driverID);
}

// Custom function for our Delivery Dispatch System:
// Pulls the root node (the highest availability score) out of the Treap
Driver* Treap::extractMostAvailable() {
    if (root == nullptr) {
        return nullptr;
    }
    
    Driver* topDriver = root->data;
    root = deleteHelper(root, topDriver->driverID);
    
    return topDriver;
}

bool Treap::isEmpty() {
    return root == nullptr;
}

void Treap::printHelper(TreapNode* root, int space) {
    if (root == nullptr) {
        return;
    }

    int spacing = 12;
    space += spacing;

    printHelper(root->right, space);

    cout << endl;
    for (int i = spacing; i < space; i++) {
        cout << " ";
    }
    cout << "Dr." << root->data->driverName << "[ID:" << root->data->driverID << "|Avl:" << root->data->availabilityScore << "]\n";

    printHelper(root->left, space);
}

void Treap::printTreap() {
    cout << "\n--- Available Drivers (Treap) ---" << endl;
    if (root == nullptr) {
        cout << "No drivers available." << endl;
    } else {
        printHelper(root, 0);
    }
    cout << "---------------------------------\n" << endl;
}