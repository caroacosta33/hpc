#ifndef PRINTS
#define PRINTS

#include "../types.h"
#include <iostream>


void printPackage(Package package) {
    std::cout << "Package ID: " << package.id << std::endl;
    std::cout << "Depth (cm): " << package.depth_cm << std::endl;
    std::cout << "Height (cm): " << package.height_cm << std::endl;
    std::cout << "Width (cm): " << package.width_cm << std::endl;
}

void printStop(Stop stop) {
    std::cout << "Stop ID: " << stop.id << std::endl;
    std::cout << "Latitude: " << stop.lat << std::endl;
    std::cout << "Longitude: " << stop.lng << std::endl;
    std::cout << "Packages:" << std::endl;
    for (const auto& package : stop.packages) {
        printPackage(package);
    }
    std::cout << "--------" << std::endl;
}

void printStop(Station station) {
    std::cout << "Station ID: " << station.id << std::endl;
    std::cout << "Latitude: " << station.lat << std::endl;
    std::cout << "Longitude: " << station.lng << std::endl;
    std::cout << "--------" << std::endl;
}

void printTruck(Truck truck) {
    std::cout << "Truck ID: " << truck.id << std::endl;
    std::cout << "Capacity: " << truck.capacity << std::endl;
    std::cout << "--------" << std::endl;
}

void printTruckRoute(TruckRoute route) {
    std::cout << "Truck ID: " << route.truckId << "\n"
              << "Route: ";
    for (const auto& stopId : route.route) {
        std::cout << stopId << " ";
    }
    std::cout << "\nUsed Capacity: " << route.usedCapacity << "\n"
              << "Distance Traveled: " << route.distanceTraveled << "\n";
}

void printSolution(Solution solution) {
    std::cout << "Solution Evaluation Value: " << solution.evaluationValue << "\n"
              << "Routes:\n";
    for (const auto& route : solution.routes) {
        printTruckRoute(route);
        std::cout << "\n";
    }
    std::cout << "Unvisited Stops:\n";
    for (const auto& stopId : solution.unvisitedStops) {
        std::cout << "  " << stopId << "\n";
    }
    std::cout << "--------" << std::endl;
}

#endif