#include <cassert>
#include <iostream>
#include <thread>
#include <mpi.h>
#include <vector>

#define PERF_START(id) \
    auto start_##id = std::chrono::high_resolution_clock::now();

#define PERF_END(id) \
    auto end_##id = std::chrono::high_resolution_clock::now();

#define PERF_RESULT(id) \
    std::chrono::duration_cast<std::chrono::microseconds>(end_##id - start_##id).count()

#define N 10e6

int main(int argc, char** argv) {
    std::vector<int> data;
    data.resize(N);    
    for(int i = 0; i < N; i++) {
        data[i] = rand() % 100;
    }

    int serialSum = 0;
    PERF_START(serial)
    for(int v : data) {
        serialSum += v;
    }
    PERF_END(serial)

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int localSize = N / size;
    int localSum = 0;
    std::vector<int> localData(localSize);


    PERF_START(parallel)
    MPI_Scatter(data.data(), localSize, MPI_INT, localData.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

    for(int v : localData) {
        localSum += v;
    }

    int globalSum = 0;    
    MPI_Reduce(&localSum, &globalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        PERF_END(parallel)
        assert(globalSum == serialSum);
        std::cout << "Sum: " << globalSum << std::endl;
        std::cout << "Serial: " << PERF_RESULT(serial) << "mcs" << std::endl;
        std::cout << "Parallel: " << PERF_RESULT(parallel) << "mcs" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
