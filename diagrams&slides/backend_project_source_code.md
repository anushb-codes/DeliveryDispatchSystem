# Delivery Dispatch System - Backend Source Code

This document contains the complete C++ backend source code for the Delivery Dispatch System, utilizing Advanced Data Structures (Binomial Heap, AVL Tree, and Treap) to achieve $O(\log n)$ efficiency.

## Directory Structure
```text
dispatch-backend/
├── include/
│   ├── Entities.h
│   ├── BinomialHeap.h
│   ├── AVLTree.h
│   ├── Treap.h
│   └── DispatchSystem.h
├── src/
│   ├── BinomialHeap.cpp
│   ├── AVLTree.cpp
│   ├── Treap.cpp
│   └── DispatchSystem.cpp
└── main.cpp
```

---

## 1. Core Entities (`include/Entities.h`)
```cpp
#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>

struct Order {
    int orderID;
    std::string customerName;
    int priority;
    int assignedDriverID;

    Order(int id, std::string name, int p) {
        orderID = id;
        customerName = name;
        priority = p;
        assignedDriverID = -1;
    }
};

struct Driver {
    int driverID;
    std::string driverName;
    int availabilityScore;

    Driver(int id, std::string name, int score) {
        driverID = id;
        driverName = name;
        availabilityScore = score;
    }
};

#endif
```

---

## 2. Priority Queue Engine (Binomial Heap)

### `include/BinomialHeap.h`
```cpp
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
    void collectOrders(HeapNode* node, std::vector<Order*>& list);

public:
    BinomialHeap();
    ~BinomialHeap();

    void insert(Order* order);
    Order* extractMin();
    bool isEmpty();
    void printHeap();
    std::vector<Order*> getPendingOrders();
};

#endif
```

### `src/BinomialHeap.cpp`
```cpp
#include "../include/BinomialHeap.h"

using namespace std;

BinomialHeap::BinomialHeap() {
    head = nullptr;
}

BinomialHeap::~BinomialHeap() {
    destroyTree(head);
}

void BinomialHeap::destroyTree(HeapNode* node) {
    if (node != nullptr) {
        destroyTree(node->sibling);
        destroyTree(node->child);
        delete node;
    }
}

void BinomialHeap::link(HeapNode* y, HeapNode* z) {
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree = z->degree + 1;
}

HeapNode* BinomialHeap::mergeRoots(HeapNode* h1, HeapNode* h2) {
    if (h1 == nullptr) return h2;
    if (h2 == nullptr) return h1;

    HeapNode* res = nullptr;
    HeapNode** pos = &res;

    while (h1 != nullptr && h2 != nullptr) {
        if (h1->degree <= h2->degree) {
            *pos = h1;
            h1 = h1->sibling;
        } else {
            *pos = h2;
            h2 = h2->sibling;
        }
        pos = &((*pos)->sibling);
    }

    if (h1 != nullptr) *pos = h1;
    else *pos = h2;

    return res;
}

HeapNode* BinomialHeap::unionHeaps(HeapNode* h1, HeapNode* h2) {
    if (h1 == nullptr && h2 == nullptr) return nullptr;

    HeapNode* res = mergeRoots(h1, h2);
    HeapNode* prev = nullptr;
    HeapNode* curr = res;
    HeapNode* next = curr->sibling;

    while (next != nullptr) {
        if ((curr->degree != next->degree) || (next->sibling != nullptr && next->sibling->degree == curr->degree)) {
            prev = curr;
            curr = next;
        } else {
            if (curr->data->priority <= next->data->priority) {
                curr->sibling = next->sibling;
                link(next, curr);
            } else {
                if (prev == nullptr) {
                    res = next;
                } else {
                    prev->sibling = next;
                }
                link(curr, next);
                curr = next;
            }
        }
        next = curr->sibling;
    }
    return res;
}

void BinomialHeap::insert(Order* order) {
    BinomialHeap tempHeap;
    tempHeap.head = new HeapNode(order);
    head = unionHeaps(head, tempHeap.head.head);
    tempHeap.head = nullptr;
}

Order* BinomialHeap::extractMin() {
    if (head == nullptr) return nullptr;

    HeapNode* minNode = head;
    HeapNode* minPrev = nullptr;
    HeapNode* curr = head;
    HeapNode* prev = nullptr;
    int minPriority = head->data->priority;

    while (curr != nullptr) {
        if (curr->data->priority < minPriority) {
            minPriority = curr->data->priority;
            minNode = curr;
            minPrev = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    if (minPrev == nullptr) {
        head = minNode->sibling;
    } else {
        minPrev->sibling = minNode->sibling;
    }

    HeapNode* child = minNode->child;
    HeapNode* revChild = nullptr;

    while (child != nullptr) {
        HeapNode* next = child->sibling;
        child->sibling = revChild;
        child->parent = nullptr;
        revChild = child;
        child = next;
    }

    BinomialHeap tempHeap;
    tempHeap.head = revChild;
    head = unionHeaps(head, tempHeap.head);

    Order* extractedOrder = minNode->data;
    delete minNode;
    return extractedOrder;
}

bool BinomialHeap::isEmpty() {
    return head == nullptr;
}

void BinomialHeap::printTree(HeapNode* node, string prefix, bool isTail) {
    if (node == nullptr) return;
    cout << prefix << (isTail ? "└── " : "├── ") << "Order #" << node->data->orderID << " (Priority: " << node->data->priority << ")" << endl;
    
    HeapNode* child = node->child;
    while (child != nullptr) {
        printTree(child, prefix + (isTail ? "    " : "│   "), child->sibling == nullptr);
        child = child->sibling;
    }
}

void BinomialHeap::printHeap() {
    cout << "\n--- Pending Orders Queue (Binomial Heap) ---" << endl;
    if (head == nullptr) {
        cout << "Queue is empty." << endl;
        return;
    }
    HeapNode* curr = head;
    while (curr != nullptr) {
        printTree(curr, "", curr->sibling == nullptr);
        curr = curr->sibling;
    }
    cout << "--------------------------------------------\n" << endl;
}

void BinomialHeap::collectOrders(HeapNode* node, std::vector<Order*>& list) {
    if (node == nullptr) return;
    list.push_back(node->data);
    HeapNode* child = node->child;
    while (child != nullptr) {
        collectOrders(child, list);
        child = child->sibling;
    }
}

std::vector<Order*> BinomialHeap::getPendingOrders() {
    std::vector<Order*> list;
    HeapNode* curr = head;
    while (curr != nullptr) {
        collectOrders(curr, list);
        curr = curr->sibling;
    }
    return list;
}
```

---

## 3. Active Tracking Engine (AVL Tree)

### `include/AVLTree.h`
```cpp
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
    
    void collectOrders(AVLNode* node, std::vector<Order*>& list);

public:
    AVLTree();
    ~AVLTree();

    void insert(Order* order);
    void remove(int orderID);
    Order* search(int orderID);
    void printTree();
    std::vector<Order*> getActiveDeliveries();
};

#endif
```

### `src/AVLTree.cpp`
```cpp
#include "../include/AVLTree.h"

using namespace std;

AVLTree::AVLTree() { root = nullptr; }

AVLTree::~AVLTree() { destroyTree(root); }

void AVLTree::destroyTree(AVLNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node; 
    }
}

int AVLTree::getHeight(AVLNode* node) {
    if (node == nullptr) return 0;
    return node->height;
}

int AVLTree::getBalanceFactor(AVLNode* node) {
    if (node == nullptr) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

AVLNode* AVLTree::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    return x;
}

AVLNode* AVLTree::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    return y;
}

AVLNode* AVLTree::insertHelper(AVLNode* node, Order* order) {
    if (node == nullptr) return new AVLNode(order);

    if (order->orderID < node->data->orderID) node->left = insertHelper(node->left, order);
    else if (order->orderID > node->data->orderID) node->right = insertHelper(node->right, order);
    else return node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalanceFactor(node);

    if (balance > 1 && order->orderID < node->left->data->orderID) return rightRotate(node);
    if (balance < -1 && order->orderID > node->right->data->orderID) return leftRotate(node);
    if (balance > 1 && order->orderID > node->left->data->orderID) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && order->orderID < node->right->data->orderID) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

void AVLTree::insert(Order* order) {
    root = insertHelper(root, order);
}

AVLNode* AVLTree::getMinValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != nullptr) current = current->left;
    return current;
}

AVLNode* AVLTree::deleteHelper(AVLNode* root, int orderID) {
    if (root == nullptr) return root;

    if (orderID < root->data->orderID) root->left = deleteHelper(root->left, orderID);
    else if (orderID > root->data->orderID) root->right = deleteHelper(root->right, orderID);
    else {
        if (root->left == nullptr || root->right == nullptr) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else *root = *temp;
            delete temp;
        } else {
            AVLNode* temp = getMinValueNode(root->right);
            root->data = temp->data;
            root->right = deleteHelper(root->right, temp->data->orderID);
        }
    }

    if (root == nullptr) return root;

    root->height = 1 + max(getHeight(root->left), getHeight(root->right));
    int balance = getBalanceFactor(root);

    if (balance > 1 && getBalanceFactor(root->left) >= 0) return rightRotate(root);
    if (balance > 1 && getBalanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalanceFactor(root->right) <= 0) return leftRotate(root);
    if (balance < -1 && getBalanceFactor(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

void AVLTree::remove(int orderID) {
    root = deleteHelper(root, orderID);
}

AVLNode* AVLTree::searchHelper(AVLNode* root, int orderID) {
    if (root == nullptr || root->data->orderID == orderID) return root;
    if (root->data->orderID < orderID) return searchHelper(root->right, orderID);
    return searchHelper(root->left, orderID);
}

Order* AVLTree::search(int orderID) {
    AVLNode* result = searchHelper(root, orderID);
    if (result != nullptr) return result->data;
    return nullptr;
}

void AVLTree::printHelper(AVLNode* root, int space) {
    if (root == nullptr) return;
    int spacing = 10;
    space += spacing;
    printHelper(root->right, space);
    cout << endl;
    for (int i = spacing; i < space; i++) cout << " ";
    cout << "ID:" << root->data->orderID << "(Dr:" << root->data->assignedDriverID << ")\n";
    printHelper(root->left, space);
}

void AVLTree::printTree() {
    cout << "\n--- Active Deliveries (AVL Tree) ---" << endl;
    if (root == nullptr) cout << "No active deliveries." << endl;
    else printHelper(root, 0);
    cout << "------------------------------------\n" << endl;
}

void AVLTree::collectOrders(AVLNode* node, std::vector<Order*>& list) {
    if (node == nullptr) return;
    collectOrders(node->left, list);
    list.push_back(node->data);
    collectOrders(node->right, list);
}

std::vector<Order*> AVLTree::getActiveDeliveries() {
    std::vector<Order*> list;
    collectOrders(root, list);
    return list;
}
```

---

## 4. Driver Load Balancer (Treap)

### `include/Treap.h`
```cpp
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
    
    void collectDrivers(TreapNode* node, std::vector<Driver*>& list);

public:
    Treap();
    ~Treap();

    void insert(Driver* driver);
    void remove(int driverID);
    Driver* extractMostAvailable();
    bool isEmpty();
    void printTreap();
    
    std::vector<Driver*> getAvailableDrivers();
};

#endif
```

### `src/Treap.cpp`
```cpp
#include "../include/Treap.h"

using namespace std;

Treap::Treap() { root = nullptr; }

Treap::~Treap() { destroyTree(root); }

void Treap::destroyTree(TreapNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
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
    if (node == nullptr) return new TreapNode(driver);

    if (driver->driverID <= node->data->driverID) {
        node->left = insertHelper(node->left, driver);
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
    if (root == nullptr) return root;

    if (driverID < root->data->driverID) {
        root->left = deleteHelper(root->left, driverID);
    } else if (driverID > root->data->driverID) {
        root->right = deleteHelper(root->right, driverID);
    } else {
        if (root->left == nullptr) {
            TreapNode* temp = root->right;
            delete root;
            root = temp;
        } else if (root->right == nullptr) {
            TreapNode* temp = root->left;
            delete root;
            root = temp;
        } else if (root->left->data->availabilityScore < root->right->data->availabilityScore) {
            root = leftRotate(root);
            root->left = deleteHelper(root->left, driverID);
        } else {
            root = rightRotate(root);
            root->right = deleteHelper(root->right, driverID);
        }
    }
    return root;
}

void Treap::remove(int driverID) {
    root = deleteHelper(root, driverID);
}

Driver* Treap::extractMostAvailable() {
    if (root == nullptr) return nullptr;
    Driver* bestDriver = root->data;
    root = deleteHelper(root, bestDriver->driverID);
    return bestDriver;
}

bool Treap::isEmpty() {
    return root == nullptr;
}

void Treap::printHelper(TreapNode* root, int space) {
    if (root == nullptr) return;
    int spacing = 10;
    space += spacing;
    printHelper(root->right, space);
    cout << endl;
    for (int i = spacing; i < space; i++) cout << " ";
    cout << root->data->driverName << "(Sc:" << root->data->availabilityScore << ")\n";
    printHelper(root->left, space);
}

void Treap::printTreap() {
    cout << "\n--- Available Drivers (Treap) ---" << endl;
    if (root == nullptr) cout << "No drivers available." << endl;
    else printHelper(root, 0);
    cout << "---------------------------------\n" << endl;
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
```

---

## 5. System Controller (`DispatchSystem`)

### `include/DispatchSystem.h`
```cpp
#ifndef DISPATCH_SYSTEM_H
#define DISPATCH_SYSTEM_H

#include "Entities.h"
#include <string>
#include <vector>

class BinomialHeap;
class AVLTree;
class Treap;

class DispatchSystem {
private:
    BinomialHeap* pendingOrdersQueue; 
    AVLTree* activeDeliveries;        
    Treap* availableDrivers;          

public:
    DispatchSystem();
    ~DispatchSystem();

    Order* receiveNewOrder(int orderID, std::string customerName, int priority);
    Driver* registerDriver(int driverID, std::string driverName);
    
    Order* dispatchNextDelivery();
    Driver* markDeliveryComplete(int orderID);
    Order* trackActiveOrder(int orderID);
    
    std::vector<Order*> getPendingOrders();
    std::vector<Order*> getActiveDeliveries();
    std::vector<Driver*> getAvailableDrivers();
};

#endif
```

### `src/DispatchSystem.cpp`
```cpp
#include "../include/DispatchSystem.h"
#include "../include/BinomialHeap.h"
#include "../include/AVLTree.h"
#include "../include/Treap.h"
#include <cstdlib>

using namespace std;

DispatchSystem::DispatchSystem() {
    pendingOrdersQueue = new BinomialHeap();
    activeDeliveries = new AVLTree();
    availableDrivers = new Treap();
}

DispatchSystem::~DispatchSystem() {
    delete pendingOrdersQueue;
    delete activeDeliveries;
    delete availableDrivers;
}

Order* DispatchSystem::receiveNewOrder(int orderID, string customerName, int priority) {
    Order* newOrder = new Order(orderID, customerName, priority);
    pendingOrdersQueue->insert(newOrder);
    return newOrder;
}

Driver* DispatchSystem::registerDriver(int driverID, string driverName) {
    int randomScore = rand() % 100; 
    Driver* newDriver = new Driver(driverID, driverName, randomScore);
    availableDrivers->insert(newDriver);
    return newDriver;
}

Order* DispatchSystem::dispatchNextDelivery() {
    if (pendingOrdersQueue->isEmpty() || availableDrivers->isEmpty()) {
        return nullptr;
    }

    Order* urgentOrder = pendingOrdersQueue->extractMin();
    Driver* assignedDriver = availableDrivers->extractMostAvailable();

    if (urgentOrder != nullptr && assignedDriver != nullptr) {
        urgentOrder->assignedDriverID = assignedDriver->driverID;
        activeDeliveries->insert(urgentOrder);
        delete assignedDriver; 
        return urgentOrder;
    }
    return nullptr;
}

Driver* DispatchSystem::markDeliveryComplete(int orderID) {
    Order* completedOrder = activeDeliveries->search(orderID);
    
    if (completedOrder == nullptr) {
        return nullptr;
    }

    int dID = completedOrder->assignedDriverID;
    string dName = "Driver_" + to_string(dID); 
    
    activeDeliveries->remove(orderID);
    
    int newScore = rand() % 100;
    Driver* returningDriver = new Driver(dID, dName, newScore);
    availableDrivers->insert(returningDriver);
    
    delete completedOrder;
    return returningDriver;
}

Order* DispatchSystem::trackActiveOrder(int orderID) {
    return activeDeliveries->search(orderID);
}

vector<Order*> DispatchSystem::getPendingOrders() { return pendingOrdersQueue->getPendingOrders(); }
vector<Order*> DispatchSystem::getActiveDeliveries() { return activeDeliveries->getActiveDeliveries(); }
vector<Driver*> DispatchSystem::getAvailableDrivers() { return availableDrivers->getAvailableDrivers(); }
```

---

## 6. Server Initialization (`main.cpp`)

```cpp
// 1. Force Windows 10 targeting to satisfy cpp-httplib
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 
#endif

// 2. Include httplib FIRST so it handles Windows socket headers correctly
#include "include/httplib.h"
#include "include/json.hpp"
#include "include/DispatchSystem.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
using json = nlohmann::json;

// Helper function to handle CORS for local React development
void setCORS(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
}

// Generates a massive dataset to test the O(log n) data structures
void preloadSystemData(DispatchSystem& dispatchCenter) {
    srand(time(0)); 

    vector<string> driverNames = {
        "Aditya", "Bhoomi", "Rahul", "Priya", "John", 
        "Sarah", "Vikram", "Sneha", "Heramb", "Ishaan", 
        "Neha", "Dhruv", "Kabir", "Aisha", "Sumaer", 
        "Advait", "Sophia", "Zara", "Raj", "Anita"
    };

    vector<string> customerNames = {
        "TechCorp Labs", "Central Cafe", "Alice Smith", "Bob Johnson", 
        "Delta Industries", "Eve Adams", "Frank Wright", "Grace Hopper", 
        "City Hospital", "University Campus", "Zara Clothing", "Yash Patel"
    };

    cout << "Injecting mock data into data structures..." << endl;
    
    for (int i = 0; i < driverNames.size(); i++) {
        dispatchCenter.registerDriver(200 + i, driverNames[i]);
    }

    for (int i = 0; i < 50; i++) {
        string customer = customerNames[rand() % customerNames.size()] + " (Bldg " + to_string((rand() % 99) + 1) + ")";
        int priority = (rand() % 3) + 1; 
        dispatchCenter.receiveNewOrder(1000 + i, customer, priority);
    }

    cout << "Successfully loaded 20 drivers and 50 pending orders." << endl;
}

int main() {
    httplib::Server svr;
    DispatchSystem dispatchCenter;

    preloadSystemData(dispatchCenter);

    svr.Options(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        res.status = 200;
    });

    svr.Get("/api/state", [&](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        json response = {
            {"pendingOrders", json::array()},
            {"activeDeliveries", json::array()},
            {"availableDrivers", json::array()}
        };

        for (Order* o : dispatchCenter.getPendingOrders()) {
            response["pendingOrders"].push_back({{"id", o->orderID}, {"customer", o->customerName}, {"priority", o->priority}});
        }
        for (Order* o : dispatchCenter.getActiveDeliveries()) {
            response["activeDeliveries"].push_back({{"id", o->orderID}, {"customer", o->customerName}, {"driverID", o->assignedDriverID}});
        }
        for (Driver* d : dispatchCenter.getAvailableDrivers()) {
            response["availableDrivers"].push_back({{"id", d->driverID}, {"name", d->driverName}, {"score", d->availabilityScore}});
        }
        
        res.set_content(response.dump(), "application/json");
    });

    svr.Post("/api/orders", [&](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        auto body = json::parse(req.body);
        
        Order* newOrder = dispatchCenter.receiveNewOrder(
            body["id"].get<int>(), 
            body["customer"].get<string>(), 
            body["priority"].get<int>()
        );
        
        json response = {{"status", "success"}, {"id", newOrder->orderID}};
        res.set_content(response.dump(), "application/json");
    });

    svr.Post("/api/dispatch", [&](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        Order* dispatched = dispatchCenter.dispatchNextDelivery();
        
        if (dispatched == nullptr) {
            res.status = 400;
            res.set_content(json({{"error", "Queue empty or no drivers available."}}).dump(), "application/json");
            return;
        }
        
        json response = {{"status", "success"}, {"orderID", dispatched->orderID}, {"driverID", dispatched->assignedDriverID}};
        res.set_content(response.dump(), "application/json");
    });

    cout << "Starting C++ Dispatch Server on http://localhost:8080..." << endl;
    svr.listen("localhost", 8080);

    return 0;
}
```