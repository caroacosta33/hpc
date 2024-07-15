#include <iostream>
#include <cmath>

// Function to convert degrees to radians
double deg2rad(double deg) {
    return deg * (M_PI / 180.0);
}

// Function to calculate the distance between two points given their latitude and longitude
double getDistanceFromLatLonInKm(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Radius of the Earth in km
    double dLat = deg2rad(lat2 - lat1);
    double dLon = deg2rad(lon2 - lon1);
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(deg2rad(lat1)) * std::cos(deg2rad(lat2)) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double d = R * c; // Distance in km
    return d;
}