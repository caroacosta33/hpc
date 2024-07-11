#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <set>
#include "include/json.hpp"

using json = nlohmann::json;

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
    std::string type;
    std::vector<Package> packages;
};

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
    std::cout << "Type: " << stop.type << std::endl;
    std::cout << "Packages:" << std::endl;
    for (const auto& package : stop.packages) {
        printPackage(package);
    }
    std::cout << "--------" << std::endl;
}

void addPackageData(std::unordered_map<std::string, Stop>& stops_hash) {
    // Abrir el archivo .json
    //std::ifstream file("almrrc2021-data-training/model_apply_inputs/new_package_data.json");
    std::ifstream file("test_files/test2.json");
    // Comprobar si el archivo se abrió correctamente
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo!" << std::endl;
        throw;
    }
    
    // Crear un objeto JSON
    json j;
    // Leer los datos del archivo y almacenarlos en el objeto JSON
    file >> j;
    // Cerrar el archivo
    file.close();

    try {
        // Recorrer las claves y valores del JSON
        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            json route_json = it.value();
            for (json::iterator it2 = route_json.begin(); it2 != route_json.end(); ++it2) {
                std::string stop_id = it2.key();
                json packages_json = it2.value();
                for (json::iterator it3 = packages_json.begin(); it3 != packages_json.end(); ++it3) {
                    Package package;
                    json dimensions_json = it3.value().at("dimensions");
                    package.id = it3.key();
                    package.depth_cm = dimensions_json.at("depth_cm").get<double>();
                    package.height_cm = dimensions_json.at("height_cm").get<double>();
                    package.width_cm = dimensions_json.at("width_cm").get<double>();
                    stops_hash[stop_id].packages.push_back(package);
                }
            }
        }
        return;
    } catch (json::exception& e) {
        std::cerr << "Error al acceder a los datos del JSON: " << e.what() << std::endl;
        throw;
    }
}

void getStopsForDateAndTruckCapacities(
        std::string selected_date, 
        std::unordered_map<std::string, Stop>&  stops_hash,
        std::set<double>& truck_capacities
    ) {
    // Abrir el archivo .json
    //std::ifstream file("almrrc2021-data-training/model_apply_inputs/new_route_data.json");
    std::ifstream file("test_files/test.json");
    // Comprobar si el archivo se abrió correctamente
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo!" << std::endl;
        throw ;
    }
    
    // Crear un objeto JSON
    json j;
    // Leer los datos del archivo y almacenarlos en el objeto JSON
    file >> j;
    // Cerrar el archivo
    file.close();

    try {
        // Recorrer las claves y valores del JSON
        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            double capacity = it.value().at("executor_capacity_cm3").get<double>();
            truck_capacities.insert(capacity); 

            std::string date = it.value().at("date_YYYY_MM_DD").get<std::string>();
            if (date == selected_date) {
                json stops = it.value().at("stops");
                for (json::iterator it2 = stops.begin(); it2 != stops.end(); ++it2) {
                    Stop stop;
                    stop.id = it2.key();
                    stop.lat = it2.value().at("lat").get<double>();
                    stop.lng = it2.value().at("lng").get<double>();
                    stop.type = it2.value().at("type").get<std::string>();
                    stops_hash[stop.id] = stop;
                }
                

            }
        }
    } catch (json::exception& e) {
        std::cerr << "Error al acceder a los datos del JSON: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    try {

    std::string selected_date = "2018-08-11";

    std::unordered_map<std::string, Stop> stops_hash;
    std::set<double> truck_capacities;

    getStopsForDateAndTruckCapacities(selected_date, stops_hash, truck_capacities);
    addPackageData(stops_hash);
    
    // Imprimir los elementos del vector
    for (const auto& pair : stops_hash) {
        printStop(pair.second);
    }

    // Imprimir el conjunto de capacidades de camiones
    std::cout << "Truck Capacities: " << std::endl;
    for (const auto& value : truck_capacities) {
        std::cout << value << std::endl;
    }

    } catch (json::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
    return 0;
}
