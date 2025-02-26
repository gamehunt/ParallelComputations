#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

#define THREADS 4
#define N 10e7

#define PERF_START(id) \
    auto start_##id = std::chrono::high_resolution_clock::now();

#define PERF_END(id) \
    auto end_##id = std::chrono::high_resolution_clock::now();

#define PERF_RESULT(id) \
    std::chrono::duration_cast<std::chrono::milliseconds>(end_##id - start_##id).count()


std::vector<int> generate(int n) {
    std::srand(time(0));
    std::vector<int> result;
    for(int i = 0; i < n; i++) {
        result.push_back(rand() % 200 - 100);
    }
    return result;
}

long sum(const std::vector<int>& array) {
    long r = 0;
    for(auto e : array) {
        r += e;
    }
    return r;
}

long sum_parallel(const std::vector<int>& array) {
    long r = 0;
#pragma omp parallel for reduction(+:r)
    for(auto e : array) {
        r += e;
    }
    return r;
}

int main() {
    omp_set_num_threads(THREADS);

    std::vector<int> vec = generate(N); 

    PERF_START(serial)
    long r1 = sum(vec);
    PERF_END(serial)

    PERF_START(parallel)
    long r2 = sum_parallel(vec);
    PERF_END(parallel)

    assert(r1 == r2);

    std::cout << "SUM: " << r1 << std::endl;
    std::cout << "SERIAL: " << PERF_RESULT(serial) << "ms" << std::endl;
    std::cout << "PARALLEL: " << PERF_RESULT(parallel) << "ms" << std::endl;

    return 0;
}
