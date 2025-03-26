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
    std::chrono::duration_cast<std::chrono::milliseconds>(end_##id - start_##id).count()

#define N 500

typedef std::vector<int> Mat;
#define INDEX(i, j, c) (i * c + j)

void print(const Mat& M, int n = N, int m = N) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            std::cout << M[i * m + j] << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Mat A;
    Mat B;
    Mat C;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) {
        A.resize(N * N);
        B.resize(N * N);
        C.resize(N * N);
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                A[i * N + j] = rand() % 100;
                B[INDEX(i, j, N)] = rand() % 100;
            }
        }
        // print(A, N);
        // std::cout << std::endl;
        // print(B, N);
    }

    int localN = N * N / size;
    Mat localA(localN);

    PERF_START(par)

    MPI_Bcast(B.data(), B.size(), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(A.data(), A.size(), MPI_INT, localA.data(), localN, MPI_INT, 0, MPI_COMM_WORLD);

    Mat localRows(localN, 0);
    for (int i = 0; i < N / size; i++) {
        for(int j = 0; j < N; j++) {
            for(int k = 0; k < N; k++) {
                localRows[INDEX(i, j ,N)] += localA[INDEX(i, k, N)] * B[INDEX(k, j + rank, N)];
            }
        }
    }

    MPI_Gather(localRows.data(), localN, MPI_INT, C.data(), localN, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        PERF_END(par)
        std::cout << "Took " << PERF_RESULT(par) << "ms" << std::endl;
        // print(C, 1);
    }


    MPI_Finalize();
    return 0;
}
