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
#include <mpi.h>

#include "./types.h"
#include "./utils/prints.h"
#include "./utils/setHelpers.h"
#include "./utils/getData.h"
#include "./utils/vectorHelpers.h"
#include "./utils/distanceHelpers.h"
#include "./utils/serialization.h"
#include "./utils/MPIHelpers.h"

// Funcion de evaluacion
double evaluateSolution(Solution solution) {
    double totalDistanceTraveled = 0;
    for (const auto& truckRoutes : solution.routes) {
        totalDistanceTraveled += truckRoutes.distanceTraveled;
    }
    return totalDistanceTraveled;
}

// precond: la truckRoute no esta vaci, tiene al menos una esatcion
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
    double maxDouble = std::numeric_limits<double>::max();
    double localMinValue = maxDouble;
    double globalMinValue = maxDouble;
    Solution minSolution;
    try {
        
        // Initializar MPI
        MPI_Init(NULL, NULL);
        int rank, size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        MPI_Group worldGroup, slavesGroup;
        MPI_Comm slavesComm;

        // Obtener el grupo del comunicador global
        MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
        // Excluir el proceso con rank 0
        int ranksToExclude[1] = {0};
        MPI_Group_excl(worldGroup, 1, ranksToExclude, &slavesGroup);
        // Crear un nuevo comunicador
        MPI_Comm_create(MPI_COMM_WORLD, slavesGroup, &slavesComm);

        if (rank == 0) {
            auto start = std::chrono::system_clock::now();
            // PARAMETROS
            int truckQty = 2; // Numero de camiones
            std::string selectedDate = "2018-08-11";
            int maxLevel = 2; // Nivel hasta el que se expande con BFS
            int maxStops = 9; // Total 654

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
                totalPackagesCapacity += getVolumeFromStop(pair.second);
            }

            std::cout << "totalTrucksCapacity: " << totalTrucksCapacity << std::endl;
            std::cout << "totalPackagesCapacity: " << totalPackagesCapacity << std::endl;
            if (totalTrucksCapacity < totalPackagesCapacity) {
                std::cout << "Trucks capacity is not enough for the packages volume" << std::endl;
                return 0;
            }


            // BFS HASTA NIVEL 
            std::vector<Solution> partialSolutions = bfs(stopsHash, stationHash, truckHash, allStops, maxLevel);
            std::cout << "BFS expansion qty: " << partialSolutions.size() << std::endl;
            // for (const auto& sol : solutions) {
            //     printSolution(sol);
            // }
            auto afterBfs = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedBfsSeconds = afterBfs-start;
            std::cout << "Elapsed Bfs time: " << elapsedBfsSeconds.count() << "s" << std::endl;

            auto serializedStops = serialize_map(stopsHash);
            auto serializedStations = serialize_map(stationHash);
            auto serializedTrucks = serialize_map(truckHash);

            broadcast_data(serializedStops, 0, rank);
            broadcast_data(serializedStations, 0, rank);
            broadcast_data(serializedTrucks, 0, rank);

            // Envío inicial de tareas a todos los procesos
            for (int i = 1; i < size; ++i) {
                std::cout << "Envio de tareas iniciales: " << i << std::endl;
                std::vector<char> buffer = serialize(partialSolutions.back());
                partialSolutions.pop_back();
                MPI_Send(buffer.data(), buffer.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }

            // Distribuir tareas restantes
            int completed_task;
            MPI_Status status;
            while (!partialSolutions.empty()) {
                std::cout << "Esperando tarea completada " << std::endl;
                MPI_Recv(&completed_task, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                std::cout << "Tarea completada " << std::endl;

                int sender = status.MPI_SOURCE;
                if (!partialSolutions.empty()) {
                    std::cout << "Envio nueva tarea " << std::endl;
                    Solution taskToSend = partialSolutions.back();
                    partialSolutions.pop_back();
                    std::vector<char> newBuffer = serialize(taskToSend);
                    MPI_Send(newBuffer.data(), newBuffer.size(), MPI_CHAR, sender, 0, MPI_COMM_WORLD);
                }
            }
            std::cout << "Esperando ultima tarea completada " << std::endl;
            MPI_Recv(&completed_task, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            std::cout << "Ultima tarea completada " << std::endl;


            // Indicar a los procesos que ya no hay más tareas
            std::cout << "No hay mas tareas " << std::endl;
            for (int i = 1; i < size; ++i) {
                Solution noTask;
                noTask.evaluationValue = -1;
                std::vector<char> buffer = serialize(noTask);
                MPI_Send(buffer.data(), buffer.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
            
            // Recibe la solucion global
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            int count;
            MPI_Get_count(&status, MPI_CHAR, &count);

            std::vector<char> buffer(count);
            MPI_Recv(buffer.data(), buffer.size(), MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            Solution globalSolution = deserialize<Solution>(buffer);
            std::cout << "globalSolution " << std::endl;
            printSolution(globalSolution);
        } else {
            int slaveRank;
            int slaveSize;
            MPI_Comm_rank(slavesComm, &slaveRank);
            MPI_Comm_size(slavesComm, &slaveSize);

            std::vector<char> receivedStops, receivedStations, receivedTrucks;

            broadcast_data(receivedStops, 0, rank);
            broadcast_data(receivedStations, 0, rank);
            broadcast_data(receivedTrucks, 0, rank);

            auto stops = deserialize_map<std::string, Stop>(receivedStops);
            auto stations = deserialize_map<std::string, Station>(receivedStations);
            auto trucks = deserialize_map<std::string, Truck>(receivedTrucks);

            while (true) {
                MPI_Status status;
                MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

                int count;
                MPI_Get_count(&status, MPI_CHAR, &count);

                std::vector<char> buffer(count);
                MPI_Recv(buffer.data(), buffer.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                Solution solution = deserialize<Solution>(buffer);

                if (solution.evaluationValue == -1) break; // No más tareas

                // DFS PARALELIZABLE
                minSolution.evaluationValue = maxDouble;
                localMinValue = maxDouble;

                dfs(trucks, stations, stops, solution, localMinValue, minSolution);

                std::cout << "Min Solution:\n";
                printSolution(minSolution);

                MPI_Request request;
                
                int flag, min;
                //  Envio el minimo local que encontre
                for (int i = 0; i < slaveSize; ++i) {
                    if (i != slaveRank) {
                        std::cout << "envio minimo a otros esclavos " << std::endl;
                        MPI_Send(&localMinValue, 1, MPI_INT, i, 0, slavesComm);
                    }
                }
                // Recibo los minimos que me enviaron los otros procesos
                for (int i = 0; i < slaveSize; ++i) {
                    if (i != slaveRank) {
                        std::cout << "recibo minimos de otros esclavos " << std::endl;
                        int flag;
                        MPI_Status status;
                        do {
                            MPI_Iprobe(i, 0, slavesComm, &flag, &status);

                            if (flag) {
                                MPI_Recv(&min, 1, MPI_INT, i, MPI_ANY_TAG, slavesComm, &status);

                                if (min < localMinValue){
                                    localMinValue = min;
                                }
                            }
                        } while (flag);// Continue receiving until there are no more messages from this process
                    }
                }
                // MPI_Allreduce(&buffer, &globalMinValue, 1, MPI_INT, MPI_MIN, slavesComm);
                // Update the localMinValue
                std::cout << "antes " << std::endl;
                if (globalMinValue < localMinValue) {
                    localMinValue = globalMinValue;
                }
                std::cout << "despues " << std::endl;
                // FIN DFS PARALELIZABLE

                int completed_task = 1;
                MPI_Send(&completed_task, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
            // Recalculate the local_min_distance for the local_min_path
            localMinValue = 0;
            localMinValue = minSolution.evaluationValue;

            // Gather minimum distances and paths from all ranks
            Solution globalMinSolution;
            int* minValues = new int[slaveSize];
            MPI_Allgather(&localMinValue, 1, MPI_INT, minValues, 1, MPI_INT, slavesComm);

            // Find rank with global minimum distance
            int* minElement = std::min_element(minValues, minValues + slaveSize);
            int minValue = *minElement;
            int globalMinRank = minElement - minValues; 

            // Receive global minimum path from rank with global minimum distance
            if (slaveRank == globalMinRank) {
                std::vector<char> responseBuffer = serialize(minSolution);
                MPI_Send(responseBuffer.data(), responseBuffer.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        MPI_Finalize();

    } catch (json::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
    return 0;
}
