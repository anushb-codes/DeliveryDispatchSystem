#ifndef DISPATCH_SYSTEM_H
#define DISPATCH_SYSTEM_H

#include "Entities.h"
#include <string>
#include <vector>

using namespace std;

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

    Order* receiveNewOrder(int orderID, string customerName, int priority);
    Driver* registerDriver(int driverID, string driverName);
    
    Order* dispatchNextDelivery();
    Driver* markDeliveryComplete(int orderID);
    Order* trackActiveOrder(int orderID);
    
    vector<Order*> getPendingOrders();
    vector<Order*> getActiveDeliveries();
    vector<Driver*> getAvailableDrivers();
};

#endif