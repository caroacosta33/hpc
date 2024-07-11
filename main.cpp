#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <set>
#include <cstdlib> 
#include <iterator>

#include "./types.h"
#include "./utils/prints.h"
#include "./utils/setHelpers.h"
#include "./utils/getData.h"

// Busqueda de las soluciones dada un nodo
void searchTree() {

}

void searchSolutions(
    std::unordered_map<std::string, Stop> stopsHash,
    std::unordered_map<std::string, Station> stationHash,
    std::unordered_map<std::string, Truck> truckHash
) {
    // Estirar el arbol hasta cierto punto
    // Elegir un nodo "station" como inicial


    // Almacenar todos las hojas del arbol

    // Repartir los nodos dinamicamente
}

int main() {
    try {

        // Numero de camiones
        int truckQty = 10;
        std::string selectedDate = "2018-08-11";

        std::unordered_map<std::string, Stop> stopsHash;
        std::unordered_map<std::string, Station> stationHash;
        std::set<double> truckCapacities;

        getStopsForDateAndTruckCapacities(selectedDate, stopsHash, stationHash, truckCapacities);
        addPackageData(stopsHash);
        
        // Imprimir los elementos del vector
        for (const auto& pair : stopsHash) {
            printStop(pair.second);
        }

        // Imprimir el conjunto de capacidades de camiones
        std::cout << "Truck Capacities: " << std::endl;
        for (const auto& value : truckCapacities) {
            std::cout << value << std::endl;
        }

        // Crea un hash con truckQty camiones de capacidad aleatoria (entre las capacidades conocidas de los camiones)
        std::unordered_map<std::string, Truck> truckHash;
        int truckCapacityQty = truckCapacities.size();
        for(int i = 0; i < truckQty; i++) {
            int rnd = rand() % truckCapacityQty; // Genera numero random entre 0 y (truckCapacityQty - 1)
            std::string truckId = "TruckId" + std::to_string(i);
            Truck truckToSave;
            truckToSave.id = truckId;
            truckToSave.capacity = getNthValue(truckCapacities, rnd);
            truckHash[truckId] = truckToSave;
        };
        // std::cout << "Trucks: " << std::endl;
        // for (const auto& pair : truckHash) {
        //     printTruck(pair.second);
        // }

        // Correr el algoritmo

    } catch (json::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
    return 0;
}
