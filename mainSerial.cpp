#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <set>
#include <cstdlib> 
#include <iterator>
#include <limits>
#include <chrono>
#include <ctime>
#include <numeric>

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

// precond: la truckRoute no esta vacia, tiene al menos una estacion
void insertStopInTruckRoute(
    std::unordered_map<std::string, Stop> stopsHash,
    std::unordered_map<std::string, Station> stationHash,
    Stop stop,
    TruckRoute& truckRoute
){
    double pkgVolume = getVolumeFromStop(stop);
    std::string prevStopId = truckRoute.route.back();
    double prevStopLat;
    double prevStopLng;
    if (truckRoute.route.size() == 1) { // Solo visitó una estacion
        prevStopLat = stationHash[prevStopId].lat;
        prevStopLng = stationHash[prevStopId].lng;
    } else {
        prevStopLat = stopsHash[prevStopId].lat;
        prevStopLng = stopsHash[prevStopId].lng;
    }
    double distanceBetweenStops = getDistanceFromLatLonInKm(
        prevStopLat,
        prevStopLng,
        stop.lat,
        stop.lng
    );

    truckRoute.usedCapacity += pkgVolume;
    truckRoute.distanceTraveled += distanceBetweenStops;
    truckRoute.route.push_back(stop.id);
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
                    insertStopInTruckRoute(stopsHash, stationHash, actualStop, editTruckRoute);
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
    for(int level = 1; level <= maxLevel; level++) {
        std::vector<Solution> levelSolutions;
        for(int j = 0; j < solutions.size(); j++) {
            std::vector<Solution> partialSolution = bfs_step(stopsHash, stationHash, truckHash, solutions[j]);
            levelSolutions.insert(levelSolutions.end(), partialSolution.begin(), partialSolution.end()); 
        };
        solutions = levelSolutions;
    };
    return solutions;
}

void dfs(std::unordered_map<std::string, Truck>& truckHash,
        std::unordered_map<std::string, Station>& stationHash,
        std::unordered_map<std::string, Stop>& stopsHash,
        Solution currSolution, double& localMinValue, Solution& localMinSolution){

    // Caso base: no quedan paradas sin visitar
    if (currSolution.unvisitedStops.empty()) {
        if (currSolution.evaluationValue < localMinSolution.evaluationValue){
            localMinSolution = currSolution;
            localMinValue = currSolution.evaluationValue;
        }
        return;
    }

    for (const auto& pair : truckHash) {
        const Truck& truck = pair.second;
        // Se intenta visitar todas las paradas que no han sido visitadas
        std::optional<TruckRoute> truckRoute = getTruckRouteById(currSolution.routes, truck.id);
        if (truckRoute) {
            for (const auto& stopId : currSolution.unvisitedStops){
                const Stop& stop = stopsHash[stopId];
                // Copia la solucion recibida por parametro
                Solution newSolution = currSolution;
                TruckRoute& editTruckRoute = refTruckRouteById(newSolution.routes, truck.id);
                double pkgVolume = getVolumeFromStop(stop);

                if (truck.capacity < pkgVolume + editTruckRoute.usedCapacity) continue;
    
                insertStopInTruckRoute(stopsHash, stationHash, stop, editTruckRoute);
                newSolution.evaluationValue = evaluateSolution(newSolution);
                if (newSolution.evaluationValue >= localMinValue) {
                    continue;
                }

                removeString(newSolution.unvisitedStops, stopId);
                dfs(truckHash, stationHash, stopsHash, newSolution, localMinValue, localMinSolution);
            }
        } else {
            for (const auto& stationPair : stationHash){
                // Copia la solucion recibida por parametro
                Solution newSolution = currSolution;

                TruckRoute newRoute;
                newRoute.truckId = truck.id;
                newRoute.route = {
                    stationPair.second.id,
                };
                newRoute.usedCapacity = 0;
                newRoute.distanceTraveled = 0;

                newSolution.routes.push_back(newRoute);
                dfs(truckHash, stationHash, stopsHash, newSolution, localMinValue, localMinSolution);
            }
        }
    }
}

int main() {
    try {
        auto start = std::chrono::system_clock::now();

        // PARAMETROS
        int truckQty = 2; // Numero de camiones
        std::string selectedDate = "2018-08-11";
        int maxLevel = 2; // Nivel hasta el que se expande con BFS
        int maxStops = 10; // Total 654

        // INICIALIZACION DE VARIABLES Y EXTRACCION DE DATOS
        std::unordered_map<std::string, Stop> stopsHash;
        std::unordered_map<std::string, Station> stationHash;
        std::set<double> truckCapacities;

        getStopsForDateAndTruckCapacities(maxStops, selectedDate, stopsHash, stationHash, truckCapacities);
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
            // truckToSave.capacity = getNthValue(truckCapacities, rnd);
            truckToSave.capacity = 400000;
            truckHash[truckId] = truckToSave;
        };

        double totalTrucksCapacity = 0;
        for(auto pair : truckHash) {
            totalTrucksCapacity += pair.second.capacity;
        }
        double totalPackagesCapacity = 0; 
        for(auto pair : stopsHash) {
            double pkgVol = getVolumeFromStop(pair.second);
            totalPackagesCapacity += getVolumeFromStop(pair.second);
        }

        std::cout << "stationsQty: " << stationHash.size() << std::endl;
        std::cout << "totalTrucksCapacity: " << totalTrucksCapacity << std::endl;
        std::cout << "totalPackagesCapacity: " << totalPackagesCapacity << std::endl;
        if (totalTrucksCapacity < totalPackagesCapacity) {
            std::cout << "Trucks capacity is not enough for the packages volume" << std::endl;
            return 0;
        }
        // BFS HASTA NIVEL 
        std::vector<Solution> solutions = bfs(stopsHash, stationHash, truckHash, allStops, maxLevel);
        std::cout << "BFS expansion qty: " << solutions.size() << std::endl;
        // for (const auto& sol : solutions) {
        //     printSolution(sol);
        // }
        auto afterBfs = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedBfsSeconds = afterBfs-start;
        std::cout << "Elapsed Bfs time: " << elapsedBfsSeconds.count() << "s" << std::endl;

        // DFS PARALELIZABLE
        Solution minSolution;
        double maxDouble = std::numeric_limits<double>::max();
        minSolution.evaluationValue = maxDouble;
        double localMinValue = maxDouble;

        // Solution solAux = solutions[0];
        // std::cout << "Sol Aux:\n";
        // printSolution(solAux);
        for(const auto& sol : solutions) {
            auto dfsStart = std::chrono::system_clock::now();
            dfs(truckHash, stationHash, stopsHash, sol, localMinValue, minSolution);
            auto dfsEnd = std::chrono::system_clock::now();
            std::chrono::duration<double> dfsTime = dfsEnd-dfsStart;
            std::cout << "dfsTime: " << dfsTime.count() << "s" << std::endl;
        }
        std::cout << "----------------------------------\n";
        std::cout << "localMinValue: " << localMinValue << std::endl;
        std::cout << "Min Solution:\n";
        printSolution(minSolution);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = end-start;
        std::time_t startTime = std::chrono::system_clock::to_time_t(start);
        std::time_t endTime = std::chrono::system_clock::to_time_t(end);

        std::cout << "Started computation at " << std::ctime(&startTime) << std::endl;
        std::cout << "Finished computation at " << std::ctime(&endTime) << std::endl;
        std::cout << "Elapsed time: " << elapsedSeconds.count() << "s" << std::endl;

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
