#ifndef TYPES
#define TYPES

#include <stdexcept>
#include <vector>

#include "./include/cereal/archives/binary.hpp"
#include "./include/cereal/types/unordered_map.hpp"
#include "./include/cereal/types/vector.hpp"
#include "./include/cereal/types/string.hpp"

struct TruckRoute {
    std::string truckId;
    std::vector<std::string> route; // ['AE', 'AH', ...]
    double usedCapacity;
    double distanceTraveled;

    template <class Archive>
    void serialize(Archive & archive) {
        archive(truckId, route, usedCapacity, distanceTraveled);
    }
};

struct Truck {
    std::string id;
    double capacity;

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(id, capacity);
    }
};

struct Package {
    std::string id;
    double depth_cm;
    double height_cm;
    double width_cm;

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(id, depth_cm, height_cm, width_cm);
    }
};

struct Stop {
    std::string id;
    double lat;
    double lng;
    std::vector<Package> packages;

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(id, lat, lng, packages);
    }
};

struct Station {
    std::string id;
    double lat;
    double lng;

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(id, lat, lng);
    }
};

// Esta solucion se refiere a una posible solucion
struct Solution {
    std::vector<TruckRoute> routes;
    std::vector<std::string> unvisitedStops;
    double evaluationValue;

    template <class Archive>
    void serialize(Archive & archive) {
        archive(routes, unvisitedStops, evaluationValue);
    }
};

#endif