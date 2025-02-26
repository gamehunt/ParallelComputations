#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

#define THREADS 4

#define PERF_START(id) \
    auto start_##id = std::chrono::high_resolution_clock::now();

#define PERF_END(id) \
    auto end_##id = std::chrono::high_resolution_clock::now();

#define PERF_RESULT(id) \
    std::chrono::duration_cast<std::chrono::milliseconds>(end_##id - start_##id).count()

typedef std::vector<std::vector<int>> matrix;

#define ROWS(m) (m.size())
#define COLUMNS(m) (m[0].size())

// const matrix first = {{1, 2, 3}, 
//                       {4, 5, 6}, 
//                       {7, 8, 9}};
//
// const matrix second = {{2, 0, 0}, 
//                        {0, 2, 1}, 
//                        {0, 0, 2}};

void print(const matrix& a) {
    for(int i = 0; i < a.size(); i++) {
        for(int j = 0; j < a[i].size(); j++) {
            std::cout << a[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

matrix multiply(const matrix& a, const matrix& b) {
    assert(COLUMNS(a) == ROWS(b));
    matrix result;
    result.resize(ROWS(a));
    for(int i = 0; i < ROWS(a); i++) {
        result[i].resize(COLUMNS(b));
    }
    for(int i = 0; i < ROWS(result); i++) {
        for(int j = 0; j < COLUMNS(result); j++) {
            for(int k = 0; k < COLUMNS(a); k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

matrix multiply_parallel(const matrix& a, const matrix& b) {
    assert(COLUMNS(a) == ROWS(b));
    matrix result;
    result.resize(ROWS(a));
#pragma omp parallel for schedule(dynamic, THREADS)
    for(int i = 0; i < ROWS(a); i++) {
        result[i].resize(COLUMNS(b));
    }
#pragma omp parallel for schedule(dynamic, THREADS)
    for(int i = 0; i < ROWS(result); i++) {
        for(int j = 0; j < COLUMNS(result); j++) {
            int sum = 0;
            for(int k = 0; k < COLUMNS(a); k++) {
                sum += a[i][k] * b[k][j];
            }
            result[i][j] = sum;
        }
    }
    return result;
}

bool compare(const matrix& a, const matrix& b) {
    if(ROWS(a) != ROWS(b) || COLUMNS(a) != COLUMNS(b)) {
        return false;
    }

    for(int i = 0; i < ROWS(a); i++) {
        for(int j = 0; j < COLUMNS(a); j++) {
            if(a[i][j] != b[i][j]) {
                return false;
            }
        }
    }

    return true;
}

matrix generate(int r, int c) {
    matrix result;
    result.resize(r);
    for(int i = 0; i < r; i++) {
        result[i].resize(c);
        for(int j = 0; j < c; j++) {
            result[i][j] = rand() % 100 - 100;
        }
    }
    return result;
}

int main() {
    srand(time(0));

    matrix first  = generate(10e3, 10e3);
    matrix second = generate(10e3, 1);

    std::cout << "generation finished" << std::endl;

    // print(first);
    // std::cout << std::endl;
    // print(second);
    // std::cout << std::endl;
    
    PERF_START(serial)
    matrix serial   = multiply(first, second);
    PERF_END(serial)
    PERF_START(parallel)
    matrix parallel = multiply_parallel(first, second); 
    PERF_END(parallel)

    assert(compare(serial, parallel));

    std::cout << "SERIAL: " << PERF_RESULT(serial) << "ms" << std::endl;
    // print(serial);
    
    // std::cout << std::endl;
    std::cout << "PARALLEL: " << PERF_RESULT(parallel) << "ms" << std::endl;

    // print(parallel);

    return 0;
}
