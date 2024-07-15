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
#include "./utils/vectorHelpers.h"
#include "./utils/distanceHelpers.h"


// Funcion de evaluacion
double evaluateSolution(Solution solution) {
    double totalDistanceTraveled = 0;
    for (const auto& truckRoutes : solution.routes) {
        totalDistanceTraveled += truckRoutes.distanceTraveled;
    }
    return totalDistanceTraveled;
}

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
        std::optional<TruckRoute> truckRoute = getTruckRouteById(solution.routes, truck.id);
        if (truckRoute) {
            // Creo un arreglo de soluciones, en donde a cada una le agrego una de las paradas sin visitar a ese camion
            // Fijarse que el camion tenga capacidad suficiente para los paquetes de esa parada
            for (const auto& stopId : solution.unvisitedStops){
                Stop actualStop = stopsHash[stopId];
                double pkgVolume = getVolumeFromStop(actualStop);
                // Copia la solucion recibida por parametro
                Solution newSolution = solution;
                TruckRoute& editTruckRoute = refTruckRouteById(newSolution.routes, truck.id);
                if (truck.capacity >= pkgVolume + editTruckRoute.usedCapacity) {
                    removeString(newSolution.unvisitedStops, stopId);
                    std::string prevStopId = editTruckRoute.route.back();
                    double prevStopLat;
                    double prevStopLng;
                    if (editTruckRoute.route.size() == 1) { // Solo visitó una estacion
                        prevStopLat = stationHash[prevStopId].lat;
                        prevStopLng = stationHash[prevStopId].lng;
                    } else {
                        prevStopLat = stopsHash[prevStopId].lat;
                        prevStopLng = stopsHash[prevStopId].lng;
                    }
                    double distanceBetweenStops = getDistanceFromLatLonInKm(
                        prevStopLat,
                        prevStopLng,
                        actualStop.lat,
                        actualStop.lng
                    );

                    editTruckRoute.usedCapacity += pkgVolume;
                    editTruckRoute.distanceTraveled += distanceBetweenStops;
                    editTruckRoute.route.push_back(stopId);
                    newSolution.evaluationValue = evaluateSolution(newSolution);

                    solutions.push_back(newSolution);
                }
            }
        } else {
            // Creo un arreglo de soluciones, en donde a cada una le agrego una de las estaciones
            for (const auto& stationPair : stationHash){
                // Copia la solucion recibida por parametro
                Solution newSolution = solution;

                TruckRoute newRoute;
                newRoute.truckId = truck.id;
                newRoute.route = {
                    stationPair.second.id,
                };
                newRoute.usedCapacity = 0;
                newRoute.distanceTraveled = 0;

                newSolution.routes.push_back(newRoute);
                solutions.push_back(newSolution);
            }
        }
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
        int truckQty = 2; // Numero de camiones
        std::string selectedDate = "2018-08-11";
        int maxLevel = 3; // Nivel hasta el que se expande con BFS

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
        for (const auto& sol : solutions) {
            printSolution(sol);
        }

        // DFS PARALELIZABLE

        //IMPRESIONES
        // Imprimir los elementos del vector
        // for (const auto& pair : stopsHash) {
        //     printStop(pair.second);
        // }

        // Imprimir el conjunto de capacidades de camiones
        // std::cout << "Truck Capacities: " << std::endl;
        // for (const auto& value : truckCapacities) {
        //     std::cout << value << std::endl;
        // }

        // std::cout << "Trucks: " << std::endl;
        // for (const auto& pair : truckHash) {
        //     printTruck(pair.second);
        // }

    } catch (json::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
    return 0;
}
