#include "../include/BinomialHeap.h"
using namespace std;

BinomialHeap::BinomialHeap() {
    head = nullptr;
}

BinomialHeap::~BinomialHeap() {
    destroyTree(head);
}

void BinomialHeap::collectOrders(HeapNode* node, vector<Order*>& list) {
    if (node == nullptr) return;
    list.push_back(node->data);
    HeapNode* child = node->child;
    while (child != nullptr) {
        collectOrders(child, list);
        child = child->sibling;
    }
}

vector<Order*> BinomialHeap::getPendingOrders() {
    vector<Order*> list;
    HeapNode* curr = head;
    while (curr != nullptr) {
        collectOrders(curr, list);
        curr = curr->sibling;
    }
    return list;
}


void BinomialHeap::destroyTree(HeapNode* node) {
    if (node != nullptr) {
        destroyTree(node->child);
        destroyTree(node->sibling);
        // We do NOT delete the Order* here, because active orders 
        // are just moving between data structures, or they will be cleaned up by DispatchSystem.
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
    HeapNode* tail = nullptr;
    HeapNode* a = h1;
    HeapNode* b = h2;

    if (a->degree <= b->degree) {
        res = a;
        a = a->sibling;
    } else {
        res = b;
        b = b->sibling;
    }
    tail = res;

    while (a != nullptr && b != nullptr) {
        if (a->degree <= b->degree) {
            tail->sibling = a;
            a = a->sibling;
        } else {
            tail->sibling = b;
            b = b->sibling;
        }
        tail = tail->sibling;
    }

    if (a != nullptr) tail->sibling = a;
    if (b != nullptr) tail->sibling = b;

    return res;
}

HeapNode* BinomialHeap::unionHeaps(HeapNode* h1, HeapNode* h2) {
    if (h1 == nullptr && h2 == nullptr) return nullptr;
    
    HeapNode* newHead = mergeRoots(h1, h2);
    if (newHead == nullptr) return nullptr;

    HeapNode* prev = nullptr;
    HeapNode* curr = newHead;
    HeapNode* next = curr->sibling;

    while (next != nullptr) {
        if ((curr->degree != next->degree) || (next->sibling != nullptr && next->sibling->degree == curr->degree)) {
            prev = curr;
            curr = next;
        } else {
            // Compare based on Order Priority (Lower number = higher urgency)
            if (curr->data->priority <= next->data->priority) {
                curr->sibling = next->sibling;
                link(next, curr);
            } else {
                if (prev == nullptr) {
                    newHead = next;
                } else {
                    prev->sibling = next;
                }
                link(curr, next);
                curr = next;
            }
        }
        next = curr->sibling;
    }
    return newHead;
}

void BinomialHeap::insert(Order* order) {
    HeapNode* newNode = new HeapNode(order);
    head = unionHeaps(head, newNode);
}

bool BinomialHeap::isEmpty() {
    return head == nullptr;
}

Order* BinomialHeap::extractMin() {
    if (head == nullptr) {
        return nullptr;
    }

    HeapNode* minNode = head;
    HeapNode* prevMin = nullptr;
    HeapNode* curr = head;
    HeapNode* prev = nullptr;

    // Find the node with the highest urgency (lowest priority number)
    while (curr != nullptr) {
        if (curr->data->priority < minNode->data->priority) {
            minNode = curr;
            prevMin = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    if (prevMin != nullptr) {
        prevMin->sibling = minNode->sibling;
    } else {
        head = minNode->sibling;
    }

    HeapNode* child = minNode->child;
    HeapNode* reversedChild = nullptr;

    // Reverse the child list to maintain Binomial Heap properties
    while (child != nullptr) {
        HeapNode* nextChild = child->sibling;
        child->sibling = reversedChild;
        child->parent = nullptr;
        reversedChild = child;
        child = nextChild;
    }

    head = unionHeaps(head, reversedChild);
    
    // Extract the exact Order pointer to return to the DispatchSystem
    Order* extractedOrder = minNode->data;
    delete minNode; 
    
    return extractedOrder;
}

void BinomialHeap::printTree(HeapNode* node, std::string prefix, bool isTail) {
    if (node == nullptr) return;
    
    cout << prefix << (isTail ? "\\-- " : "|-- ") 
              << "Order #" << node->data->orderID 
              << " (" << node->data->customerName << ", P:" << node->data->priority << ")" <<endl;
    
    HeapNode* child = node->child;
    while (child != nullptr) {
        printTree(child, prefix + (isTail ? "    " : "|   "), child->sibling == nullptr);
        child = child->sibling;
    }
}

void BinomialHeap::printHeap() {
    if (head == nullptr) {
        cout << "Dispatch Queue (Heap) is currently empty." <<endl;
        return;
    }
    cout << "\n--- Dispatch Queue (Binomial Heap) ---" <<endl;
    HeapNode* curr = head;
    while (curr != nullptr) {
        cout << "ROOT: Order #" << curr->data->orderID 
                  << " (" << curr->data->customerName << ", P:" << curr->data->priority << ")" <<endl;
        HeapNode* child = curr->child;
        while (child != nullptr) {
            printTree(child, "", child->sibling == nullptr);
            child = child->sibling;
        }
        curr = curr->sibling;
        if (curr != nullptr) {
            cout << "-" << endl;
        }
    }
    cout << "--------------------------------------\n" <<endl;
}