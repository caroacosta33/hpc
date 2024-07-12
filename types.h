#ifndef TYPES
#define TYPES

#include <stdexcept>
#include <vector>

struct TruckRoute {
    std::string truckId;
    std::vector<std::string> route; // ['AE', 'AH', ...]
    double usedCapacity;
    double distanceTraveled;
};

struct Truck {
    std::string id;
    double capacity;
};

struct Package {
    std::string id;
    double depth_cm;
    double height_cm;
    double width_cm;
};

struct Stop {
    std::string id;
    double lat;
    double lng;
    std::vector<Package> packages;
};

struct Station {
    std::string id;
    double lat;
    double lng;
};

struct Solution {
    std::vector<TruckRoute> routes;
    std::vector<std::string> unvisitedStops;
    double evaluationValue;
};

#endif