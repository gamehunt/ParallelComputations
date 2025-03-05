#include <chrono>
#include <cstring>
#include <cstdlib>
#include <omp.h>
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

#define DEAD '.'
#define ALIVE '#'
#define RANDOMIZE_FIELD 1

#define FIELD_SIZE 200
#define CELL_SIZE 4

int rows, cols;
char**  field1;
char**  field2;
char*** fields[2] = {&field1, &field2};
int active_field = 0;

void print_field(cv::Mat& image, long iteration_time) {
    image = cv::Scalar(0, 0, 0);
    const cv::Scalar color(255, 255, 255);
    int alive = 0;
    char** field = *fields[active_field];
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(field[i][j] == ALIVE) {
                alive++;
                cv::rectangle(image, cv::Point(j * CELL_SIZE, i * CELL_SIZE), cv::Point(j * CELL_SIZE + CELL_SIZE - 1, i * CELL_SIZE + CELL_SIZE - 1), color, -1);
            }  
        }
    }
    std::string text = "ALIVE: " + std::to_string(alive) + " | Iteration time: " + std::to_string(iteration_time) + "mcs";
    cv::putText(image, text, cv::Point(0, FIELD_SIZE * CELL_SIZE + 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0));
}

int count_alive_neibourghs(char** field, int x, int y) {
    int r = 0;
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            int ox = x + i;
            int oy = y + j;

            if(ox == x && oy == y || ox < 0 || oy < 0 || ox >= cols || oy >= rows) {
                continue;
            }

            if(field[oy][ox] == ALIVE) {
                r++;
            }
        }
    }
    return r;
}

void turn() {
    char** field      = *fields[active_field];
    char** field_copy = *fields[active_field == 0 ? 1 : 0];
#pragma omp parallel for
    for(int t = 0; t < rows * cols; t++) {
        int i = t / cols;
        int j = t % cols;
        int alive_n = count_alive_neibourghs(field, j, i);
        if(field[i][j] == ALIVE) {
            if(alive_n > 3) {
                field_copy[i][j] = DEAD;
            } else if (alive_n < 2) {
                field_copy[i][j] = DEAD;
            } else {
                field_copy[i][j] = field[i][j];
            }
        } else if (alive_n == 3) {
            field_copy[i][j] = ALIVE;
        } else {
            field_copy[i][j] = DEAD;
        }
    }
// #pragma omp parallel for
//     for(int i = 0; i < rows; i++) {
//         memcpy(field[i], field_copy[i], cols);
//     }
    active_field = active_field ? 0 : 1;
}

int main() {
    omp_set_num_threads(THREAD_AMOUNT);

    // initscr();
    // halfdelay(5);
    //
    // getmaxyx(stdscr, rows, cols);

    rows = cols = FIELD_SIZE;

    field1 = new char*[rows];
    field2 = new char*[rows];
    for(int i = 0; i < rows; i++) {
        field1[i] = new char[cols];
        field2[i] = new char[cols];
#ifndef RANDOMIZE_FIELD
        memset(field[i], DEAD, cols);
#else
        for(int j = 0; j < cols; j++) {
            field1[i][j] = std::rand() % 2 ? DEAD : ALIVE;
        }
#endif
    }

#ifndef RANDOMIZE_FIELD
    field[0][1] = ALIVE;
    field[1][2] = ALIVE;
    field[2][0] = ALIVE;
    field[2][1] = ALIVE;
    field[2][2] = ALIVE;
#endif

    cv::Mat image(FIELD_SIZE*CELL_SIZE + 50, FIELD_SIZE*CELL_SIZE, CV_8UC3, cv::Scalar(0, 0, 0));
    while(true) {
        PERF_START(i);
        turn();
        PERF_END(i);
        print_field(image, PERF_RESULT(i));
        imshow("root", image);
        int key = cv::waitKey(300);
        if(key == 27) {
            break;
        }
    }

    return 0;
}
