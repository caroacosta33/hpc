#ifndef GETDATA
#define GETDATA

#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include "../types.h"
#include "../include/json.hpp"
using json = nlohmann::json;

void addPackageData(std::unordered_map<std::string, Stop>& stopsHash) {
    // Abrir el archivo .json
    //std::ifstream file("almrrc2021-data-training/model_apply_inputs/new_package_data.json");
    std::ifstream file("test_files2/test2.json");
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
            json routeJson = it.value();
            for (json::iterator it2 = routeJson.begin(); it2 != routeJson.end(); ++it2) {
                std::string stopId = it2.key();
                json packagesJson = it2.value();
                for (json::iterator it3 = packagesJson.begin(); it3 != packagesJson.end(); ++it3) {
                    Package package;
                    json dimensionsJson = it3.value().at("dimensions");
                    package.id = it3.key();
                    package.depth_cm = dimensionsJson.at("depth_cm").get<double>();
                    package.height_cm = dimensionsJson.at("height_cm").get<double>();
                    package.width_cm = dimensionsJson.at("width_cm").get<double>();
                    stopsHash[stopId].packages.push_back(package);
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
        std::string selectedDate, 
        std::unordered_map<std::string, Stop>& stopsHash,
        std::unordered_map<std::string, Station>& stationsHash,
        std::set<double>& truckCapacities
    ) {
    // Abrir el archivo .json
    //std::ifstream file("almrrc2021-data-training/model_apply_inputs/new_route_data.json");
    std::ifstream file("test_files2/test.json");
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
            truckCapacities.insert(capacity); 

            std::string date = it.value().at("date_YYYY_MM_DD").get<std::string>();
            if (date == selectedDate) {
                json stops = it.value().at("stops");
                for (json::iterator it2 = stops.begin(); it2 != stops.end(); ++it2) {
                    if (it2.value().at("type").get<std::string>() == "Station") {
                        Station station;
                        station.id = it2.key();
                        station.lat = it2.value().at("lat").get<double>();
                        station.lng = it2.value().at("lng").get<double>();
                        stationsHash[station.id] = station;
                    } else { // El tipo es "Dropoff"
                        Stop stop;
                        stop.id = it2.key();
                        stop.lat = it2.value().at("lat").get<double>();
                        stop.lng = it2.value().at("lng").get<double>();
                        stopsHash[stop.id] = stop;
                    }
                }
                

            }
        }
    } catch (json::exception& e) {
        std::cerr << "Error al acceder a los datos del JSON: " << e.what() << std::endl;
        throw;
    }
}

double getVolumeFromStop(Stop stop) {
    double vol = 0;
    for (const auto& package : stop.packages) {
        vol += package.depth_cm * package.height_cm * package.width_cm;
    }
    return vol;
}

#endif