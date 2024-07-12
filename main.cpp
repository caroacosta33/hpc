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

std::vector<Solution> bfs_step(
    std::unordered_map<std::string, Stop> stopsHash,
    std::unordered_map<std::string, Station> stationHash,
    std::unordered_map<std::string, Truck> truckHash,
    Solution solution
) {
    std::vector<Solution> solutions;
    for (const auto& pair : truckHash) {
        Truck truck = pair.second;
        // if (solution.routes == )
        // si el camion tiene una ruta no vacia
            // creo un arreglo de soluciones, en donde a cada una le agrego una de las paradas sin visitar a ese camion
            // fijarse que el camion tenga capacidad suficiente para los paquetes de esa parada
        // si no
            // creo un arreglo de soluciones, en donde a cada una le agrego una de las estaciones
    }
    return solutions;
}

std::vector<Solution> bfs(
    std::unordered_map<std::string, Stop> stopsHash,
    std::unordered_map<std::string, Station> stationHash,
    std::unordered_map<std::string, Truck> truckHash,
    std::vector<std::string> allStops,
    int maxLevel
) {
    Solution initialSolution;
    initialSolution.unvisitedStops = allStops;
    initialSolution.evaluationValue = 0;

    std::vector<Solution> solutions = bfs_step(stopsHash, stationHash, truckHash, initialSolution);
    for(int level = 1; level < maxLevel; level++) {
        std::vector<Solution> levelSolutions;
        for(int j = 0; j < solutions.size(); j++) {
            std::vector<Solution> partialSolution = bfs_step(stopsHash, stationHash, truckHash, solutions[j]);
            levelSolutions.insert(levelSolutions.end(), partialSolution.begin(), partialSolution.end()); 
        };
        solutions = levelSolutions;
    };
    return solutions;
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
        // PARAMETROS
        int truckQty = 10; // Numero de camiones
        std::string selectedDate = "2018-08-11";
        int maxLevel = 10; // Nivel hasta el que se expande con BFS

        // INICIALIZACION DE VARIABLES Y EXTRACCION DE DATOS
        std::unordered_map<std::string, Stop> stopsHash;
        std::unordered_map<std::string, Station> stationHash;
        std::set<double> truckCapacities;

        getStopsForDateAndTruckCapacities(selectedDate, stopsHash, stationHash, truckCapacities);
        addPackageData(stopsHash);
    
        std::vector<std::string> allStops;
        for(auto pair : stopsHash) {
            allStops.push_back(pair.first);
        }
        std::vector<TruckRoute> solutionRoutes;

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

        // BFS HASTA NIVEL 
        std::vector<Solution> solutions = bfs(stopsHash, stationHash, truckHash, allStops, maxLevel);

        // DFS PARALELIZABLE

        //IMPRESIONES
        // Imprimir los elementos del vector
        for (const auto& pair : stopsHash) {
            printStop(pair.second);
        }

        // Imprimir el conjunto de capacidades de camiones
        std::cout << "Truck Capacities: " << std::endl;
        for (const auto& value : truckCapacities) {
            std::cout << value << std::endl;
        }

        std::cout << "Trucks: " << std::endl;
        for (const auto& pair : truckHash) {
            printTruck(pair.second);
        }

    } catch (json::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
    return 0;
}
