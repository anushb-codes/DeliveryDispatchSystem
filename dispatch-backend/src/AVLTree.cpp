#include "../include/AVLTree.h"
#include <algorithm>

using namespace std;

AVLTree::AVLTree() {
    root = nullptr;
}

AVLTree::~AVLTree() {
    destroyTree(root);
}

void AVLTree::destroyTree(AVLNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node; // We delete the node, but DispatchSystem handles the Order* data
    }
}

void AVLTree::collectOrders(AVLNode* node, vector<Order*>& list) {
    if (node == nullptr) return;
    collectOrders(node->left, list);
    list.push_back(node->data);
    collectOrders(node->right, list);
}

vector<Order*> AVLTree::getActiveDeliveries() {
    vector<Order*> list;
    collectOrders(root, list);
    return list;
}


int AVLTree::getHeight(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

int AVLTree::getBalanceFactor(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

AVLNode* AVLTree::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

AVLNode* AVLTree::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

AVLNode* AVLTree::insertHelper(AVLNode* node, Order* order) {
    if (node == nullptr) {
        return new AVLNode(order);
    }

    // Standard BST insertion using orderID as the key
    if (order->orderID < node->data->orderID) {
        node->left = insertHelper(node->left, order);
    } else if (order->orderID > node->data->orderID) {
        node->right = insertHelper(node->right, order);
    } else {
        return node; // Duplicate orderIDs are not allowed
    }

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    int balance = getBalanceFactor(node);

    // Left Left Case
    if (balance > 1 && order->orderID < node->left->data->orderID) {
        return rightRotate(node);
    }

    // Right Right Case
    if (balance < -1 && order->orderID > node->right->data->orderID) {
        return leftRotate(node);
    }

    // Left Right Case
    if (balance > 1 && order->orderID > node->left->data->orderID) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
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
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

AVLNode* AVLTree::deleteHelper(AVLNode* root, int orderID) {
    if (root == nullptr) {
        return root;
    }

    if (orderID < root->data->orderID) {
        root->left = deleteHelper(root->left, orderID);
    } else if (orderID > root->data->orderID) {
        root->right = deleteHelper(root->right, orderID);
    } else {
        // Node found
        if (root->left == nullptr || root->right == nullptr) {
            AVLNode* temp = root->left ? root->left : root->right;

            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {
                *root = *temp; 
            }
            delete temp;
        } else {
            // Node with two children: Get inorder successor
            AVLNode* temp = getMinValueNode(root->right);
            root->data = temp->data;
            root->right = deleteHelper(root->right, temp->data->orderID);
        }
    }

    if (root == nullptr) {
        return root;
    }

    root->height = 1 + max(getHeight(root->left), getHeight(root->right));

    int balance = getBalanceFactor(root);

    // Left Left Case
    if (balance > 1 && getBalanceFactor(root->left) >= 0) {
        return rightRotate(root);
    }

    // Left Right Case
    if (balance > 1 && getBalanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalanceFactor(root->right) <= 0) {
        return leftRotate(root);
    }

    // Right Left Case
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
    if (root == nullptr || root->data->orderID == orderID) {
        return root;
    }

    if (root->data->orderID < orderID) {
        return searchHelper(root->right, orderID);
    }
    
    return searchHelper(root->left, orderID);
}

Order* AVLTree::search(int orderID) {
    AVLNode* result = searchHelper(root, orderID);
    if (result != nullptr) {
        return result->data;
    }
    return nullptr;
}

void AVLTree::printHelper(AVLNode* root, int space) {
    if (root == nullptr) {
        return;
    }

    int spacing = 10;
    space += spacing;

    printHelper(root->right, space);

    cout << endl;
    for (int i = spacing; i < space; i++) {
        cout << " ";
    }
    cout << "ID:" << root->data->orderID << "(Dr:" << root->data->assignedDriverID << ")\n";

    printHelper(root->left, space);
}

void AVLTree::printTree() {
    cout << "\n--- Active Deliveries (AVL Tree) ---" << endl;
    if (root == nullptr) {
        cout << "No active deliveries." << endl;
    } else {
        printHelper(root, 0);
    }
    cout << "------------------------------------\n" << endl;
}