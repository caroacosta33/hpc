#ifndef MPI_HELPERS
#define MPI_HELPERS

#include <iostream>
#include <vector>
#include <mpi.h>

// Function to broadcast data
void broadcast_data(std::vector<char>& data, int sender_rank, int current_rank) {
    // Broadcast the size of the data
    int size = data.size();
    MPI_Bcast(&size, 1, MPI_INT, sender_rank, MPI_COMM_WORLD);

    if (size > 0) {
        // Resize the vector if this is not the sender process
        if (current_rank != sender_rank) {
            data.resize(size);
        }
        // Broadcast the actual data
        MPI_Bcast(data.data(), size, MPI_CHAR, sender_rank, MPI_COMM_WORLD);
    }
}

void send_data(const std::vector<char>& data, int destination, int tag) {
    MPI_Send(data.data(), data.size(), MPI_CHAR, destination, tag, MPI_COMM_WORLD);
}

std::vector<char> receive_data(int source, int tag) {
    MPI_Status status;
    MPI_Probe(source, tag, MPI_COMM_WORLD, &status);

    int count;
    MPI_Get_count(&status, MPI_CHAR, &count);

    std::vector<char> buffer(count);
    MPI_Recv(buffer.data(), count, MPI_CHAR, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return buffer;
}

#endif