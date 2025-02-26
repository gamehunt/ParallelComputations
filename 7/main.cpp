#include <cassert>
#include <cfloat>
#include <cmath>
#include <functional>
#include <iostream>
#include <chrono>
#include <omp.h>

#define THREADS 4
#define N 10e7
#define EPS 10e-8

#define PERF_START(id) \
    auto start_##id = std::chrono::high_resolution_clock::now();

#define PERF_END(id) \
    auto end_##id = std::chrono::high_resolution_clock::now();

#define PERF_RESULT(id) \
    std::chrono::duration_cast<std::chrono::milliseconds>(end_##id - start_##id).count()

double f(double x) {
    return std::exp(-std::pow(x, 2));
}

double integrate(std::function<double(double)> f, double a, double b) {
    double r = 0;
    double h = (b - a) / N;
    for(int i = 1; i < N; i++) {
        double xip = a + (i - 1) * h;
        double xi = a + i * h;
        double x = (xip + xi) / 2;
        r += f(x) * (xi - xip);
    }
    return r;
}

double integrate_parallel(std::function<double(double)> f, double a, double b) {
    double r = 0;
    double h = (b - a) / N;
    int n = N;
#pragma omp parallel for reduction(+:r)
    for(int i = 1; i < n; i++) {
        double xip = a + (i - 1) * h;
        double xi = a + i * h;
        double x = (xip + xi) / 2;
        r += f(x) * (xi - xip);
    }
    return r;
}

int main() {
    omp_set_num_threads(THREADS);

    PERF_START(serial)
    double r1 = integrate(f, -M_PI, M_PI);
    PERF_END(serial)

    PERF_START(parallel)
    double r2 = integrate_parallel(f, -M_PI, M_PI);
    PERF_END(parallel)

    assert(fabs(r1 - r2) < EPS);

    std::cout << "RESULT: " << r1 << std::endl;
    std::cout << "PRECISE: " << sqrt(M_PI) * erf(M_PI) << std::endl;
    std::cout << "SERIAL: " << PERF_RESULT(serial) << "ms" << std::endl;
    std::cout << "PARALLEL: " << PERF_RESULT(parallel) << "ms" << std::endl;

    return 0;
}
