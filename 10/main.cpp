#include <chrono>
#include <cstring>
#include <cstdlib>
#include <ncurses.h>
#include <omp.h>

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

int rows, cols;
char** field;
char** field_copy;

void print_field(long iteration_time) {
    clear();
    int alive = 0;
    for(int i = 0; i < rows - 1; i++) {
        for(int j = 0; j < cols; j++) {
            if(field[i][j] == ALIVE) {
                alive++;
            }
            mvaddch(i, j, field[i][j]);
        }
    }
    mvprintw(rows - 1, 0, "ALIVE CELLS: %d | ITERATION TIME: %ldms", alive, iteration_time);
    refresh();
}

int count_alive_neibourghs(int x, int y) {
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
#pragma omp parallel for
    for(int t = 0; t < rows * cols; t++) {
        int i = t / cols;
        int j = t % cols;
        int alive_n = count_alive_neibourghs(j, i);
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
#pragma omp parallel for
    for(int i = 0; i < rows; i++) {
        memcpy(field[i], field_copy[i], cols);
    }
}

int main() {
    omp_set_num_threads(THREAD_AMOUNT);

    initscr();
    halfdelay(5);

    getmaxyx(stdscr, rows, cols);

    field = new char*[rows];
    field_copy = new char*[rows];
    for(int i = 0; i < rows; i++) {
        field[i] = new char[cols];
        field_copy[i] = new char[cols];
#ifndef RANDOMIZE_FIELD
        memset(field[i], DEAD, cols);
#else
        for(int j = 0; j < cols; j++) {
            field[i][j] = std::rand() % 2 ? DEAD : ALIVE;
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

    while(getch() != 27) {
        PERF_START(i);
        turn();
        PERF_END(i);
        print_field(PERF_RESULT(i));
    }

    endwin();
    return 0;
}
