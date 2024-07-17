#ifndef VECTORHELPERS
#define VECTORHELPERS

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include "../types.h"

std::optional<TruckRoute> getTruckRouteById(const std::vector<TruckRoute>& vec, const std::string& truckId) {
    for (const auto& truckRoute : vec) {
        if (truckRoute.truckId == truckId) {
            return truckRoute;
        }
    }
    return std::nullopt;
}

// Precond: truckId esta en vec
TruckRoute& refTruckRouteById(std::vector<TruckRoute>& vec, const std::string& truckId) {
    for (auto& route : vec) {
        if (route.truckId == truckId) {
            return route;
        }
    }
    // If not found, typically handle error or throw exception
    // Here, we assume it always exists for simplicity
    throw std::runtime_error("TruckRoute not found for truckId: " + truckId);
}

void removeString(std::vector<std::string>& vec, const std::string& value) {
    vec.erase(std::remove_if(vec.begin(), vec.end(), 
                             [&value](const std::string& str) {
                                 return str == value;
                             }), 
              vec.end());
}

#endif
