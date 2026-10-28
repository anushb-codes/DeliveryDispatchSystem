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

// Function to flood the data structures with test entries
void preloadSystemData(DispatchSystem& dispatchCenter) {
    srand(time(0)); 

    vector<string> drivers = {
        "Aditya", "Karan", "Rahul", "Priya", "John", 
        "Sarah", "Vikram", "Sneha", "Heramb", "Ishaan", 
        "Neha", "Dhruv", "Kabir", "Aisha", "Sumaer"
    };

    vector<string> customers = {
        "TechCorp Labs", "Central Cafe", "Alice Smith", "Bob Johnson", 
        "Delta Industries", "Eve Adams", "Frank Wright", "Grace Hopper", 
        "City Hospital", "University Campus"
    };

    cout << "Injecting mock data into data structures..." << endl;
    
    for (int i = 0; i < drivers.size(); i++) {
        dispatchCenter.registerDriver(200 + i, drivers[i]);
    }

    // Put 30 orders into the Staging Hub and instantly merge them into the Main Hub
    for (int i = 0; i < 30; i++) {
        string customer = customers[rand() % customers.size()] + " (Bldg " + to_string((rand() % 99) + 1) + ")";
        int priority = (rand() % 3) + 1; 
        dispatchCenter.receiveNewOrder(1000 + i, customer, priority);
    }
    dispatchCenter.mergeHubs(); 

    // Put 10 orders into the Staging Hub and leave them there to demonstrate the UI
    for (int i = 30; i < 40; i++) {
        string customer = customers[rand() % customers.size()] + " (Bldg " + to_string((rand() % 99) + 1) + ")";
        int priority = (rand() % 3) + 1; 
        dispatchCenter.receiveNewOrder(1000 + i, customer, priority);
    }
}

int main() {
    httplib::Server svr;
    DispatchSystem dispatchCenter;

    // Load the bulk data
    preloadSystemData(dispatchCenter);

    // Handle Preflight OPTIONS requests for CORS
    svr.Options(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        res.status = 200;
    });

    // GET: Fetch entire system state for the React Dashboard
    svr.Get("/api/state", [&](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        json response = {
            {"pendingOrders", json::array()},
            {"stagingOrders", json::array()}, // NEW: Staging array
            {"activeDeliveries", json::array()},
            {"availableDrivers", json::array()}
        };

        for (Order* o : dispatchCenter.getPendingOrders()) {
            response["pendingOrders"].push_back({{"id", o->orderID}, {"customer", o->customerName}, {"priority", o->priority}});
        }
        for (Order* o : dispatchCenter.getStagingOrders()) {
            response["stagingOrders"].push_back({{"id", o->orderID}, {"customer", o->customerName}, {"priority", o->priority}});
        }
        for (Order* o : dispatchCenter.getActiveDeliveries()) {
            response["activeDeliveries"].push_back({{"id", o->orderID}, {"customer", o->customerName}, {"driverID", o->assignedDriverID}});
        }
        for (Driver* d : dispatchCenter.getAvailableDrivers()) {
            response["availableDrivers"].push_back({{"id", d->driverID}, {"name", d->driverName}, {"score", d->availabilityScore}});
        }
        
        res.set_content(response.dump(), "application/json");
    });

    // POST: Create a new order (now lands in Staging Hub)
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
    
    // POST: Merge Staging Hub into Main Pending Queue
    svr.Post("/api/merge", [&](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        dispatchCenter.mergeHubs();
        json response = {{"status", "success"}};
        res.set_content(response.dump(), "application/json");
    });

    // POST: Mark an active delivery as complete
    svr.Post("/api/complete", [&](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        try {
            auto body = json::parse(req.body);
            int orderId = body["id"].get<int>();
            Driver* returningDriver = dispatchCenter.markDeliveryComplete(orderId);
            
            if (returningDriver == nullptr) {
                res.status = 404;
                res.set_content(json({{"error", "Order not found in AVL Tree"}}).dump(), "application/json");
                return;
            }
            json response = {{"status", "success"}, {"driverID", returningDriver->driverID}};
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json({{"error", "Failed to parse completion request"}}).dump(), "application/json");
        }
    });

    // POST: Trigger a dispatch algorithm
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