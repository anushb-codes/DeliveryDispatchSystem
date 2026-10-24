#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>

struct Order {
    int orderID;
    std::string customerName;
    int priority; // Lower number = higher urgency (e.g., 1 for Express, 3 for Standard)
    int assignedDriverID; // Defaults to -1 until dispatched

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
    int availabilityScore; // Randomized heap priority for load balancing

    Driver(int id, std::string name, int score) {
        driverID = id;
        driverName = name;
        availabilityScore = score;
    }
};

#endif