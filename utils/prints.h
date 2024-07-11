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

#endif