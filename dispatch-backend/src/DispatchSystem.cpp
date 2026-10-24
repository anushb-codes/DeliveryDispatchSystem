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
    if (pendingOrdersQueue->isEmpty() == true || availableDrivers->isEmpty() == true) {
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