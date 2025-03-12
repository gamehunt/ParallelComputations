#include <chrono>
#include <cstring>
#include <cstdlib>
#include <omp.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define PERF_START(id) \
    auto start_##id = std::chrono::high_resolution_clock::now();

#define PERF_END(id) \
    auto end_##id = std::chrono::high_resolution_clock::now();

#define PERF_RESULT(id) \
    std::chrono::duration_cast<std::chrono::microseconds>(end_##id - start_##id).count()

#define THREAD_AMOUNT 4

constexpr int offsets[] = {0, 1, 2};

#define DEFAULT_SIZE  405
#define DEFAULT_DEPTH 4

void fractal(cv::Mat& image, cv::Point position, cv::Size size, cv::Scalar color, int depth) {
    if(depth == 1) {
        cv::rectangle(image, position, position + cv::Point(size.width, size.height), color, -1);
    } else {
#pragma omp parallel for
        for(int i = 0; i < 9; i++) {
            int ox = offsets[i / 3]; 
            int oy = offsets[i % 3]; 
            if(ox == 1 && oy == 1) {
                continue;
            }
            cv::Size newSize = size / 3;
            fractal(image, position + cv::Point(ox * newSize.width, oy * newSize.height), newSize, color, depth - 1);
        }
    }
}


int main() {
    omp_set_num_threads(THREAD_AMOUNT);

    cv::Size size;
    int depth;

    std::cout << "Enter image size: ";
    std::string in;
    getline(std::cin, in);

    if(in.empty()) {
        std::cout << "Using default size." << std::endl;
        size = cv::Size(DEFAULT_SIZE, DEFAULT_SIZE);
    } else {
        int s = std::stoi(in);
        if(s <= 0) {
            std::cout << "Invalid input." << std::endl;
            exit(1);
        } else {
            size = cv::Size(s, s);
        }
    }

    std::cout << "Enter recursion depth: ";
    getline(std::cin, in);
    
    if(in.empty()) {
        std::cout << "Using default depth." << std::endl;
        depth = DEFAULT_DEPTH;
    } else {
        depth = std::stoi(in);
        if(depth <= 0) {
            std::cout << "Invalid input." << std::endl;
            exit(1);
        } 
    }

    cv::Mat image = cv::Mat::zeros(size, CV_8UC3);
    fractal(image, cv::Point(0, 0), size, cv::Scalar(255, 255, 255), depth);

    imshow("fractal", image);
    imwrite("carpet.png", image);

    while((cv::waitKey() & 0xEFFFFF) != 27);

    return 0;
}
