#include <mpi.h>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/opencv.hpp>

#define PERF_START(id) \
    auto start_##id = std::chrono::high_resolution_clock::now();

#define PERF_END(id) \
    auto end_##id = std::chrono::high_resolution_clock::now();

#define PERF_RESULT(id) \
    std::chrono::duration_cast<std::chrono::microseconds>(end_##id - start_##id).count()

#define N 1000
#define W 1200
#define H 800

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    size -= 1;

    int rowsPerWorker = H / size;
    int localSize = rowsPerWorker * W;

    if(rank == 0) {
        cv::Mat image(H, W, CV_8UC3);
        image = cv::Scalar(255, 255, 255);
        MPI_Status status;
        int* r = new int[localSize];
        cv::Vec3b palette[] = {
            cv::Vec3b(66, 30, 15),
            cv::Vec3b(25, 7, 26),
            cv::Vec3b(9, 1, 47),
            cv::Vec3b(4, 4, 73),
            cv::Vec3b(0, 7, 100),
            cv::Vec3b(12, 44, 138),
            cv::Vec3b(24, 82, 177),
            cv::Vec3b(57, 125, 209),
            cv::Vec3b(134, 181, 229),
            cv::Vec3b(211, 236, 248),
            cv::Vec3b(241, 233, 191),
            cv::Vec3b(248, 201, 95),
            cv::Vec3b(255, 170, 0),
            cv::Vec3b(204, 128, 0),
            cv::Vec3b(153, 87, 0),
            cv::Vec3b(106, 52, 3),
        };
        for(int i = 0; i < size; i++) {
            MPI_Recv(r, localSize, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            int src = status.MPI_SOURCE - 1;
            for(int i = 0; i < rowsPerWorker; i++) {
                for(int j = 0; j < W; j++) {
                    int index = r[i * W + j];
                    cv::Vec3b col(255, 255, 255);
                    if(index != -1) {
                        col = palette[index % 16];
                    } else {
                        col = cv::Vec3b::zeros();
                    }
                    cv::Vec3b& color = image.at<cv::Vec3b>(i + src * rowsPerWorker,j);
                    color = col;
                }
            }
        }
        delete[] r;
        cv::imshow("Mandelbrot set", image);
        while((cv::waitKey() & 0xEFFFFF) != 27);
    } else {
        int* buf = new int[localSize];
        for(int i = 0; i < rowsPerWorker; i++) {
            for(int j = 0; j < W; j++) {
                int yi = i + rowsPerWorker * (rank - 1);     
                double x0 = 3.0 * j / W - 2;
                double y0 = 2.0 * yi / H - 1;
                double x = x0;
                double y = y0;
                int color = -1;
                for(int k = 0; k < N; k++) {
                    double z = x * x + y * y;
                    if(z > 4) {
                        color = k;
                        break;
                    }
                    double tmp = x;
                    x = x * x - y * y + x0;
                    y = 2 * tmp * y + y0;
                }
                buf[i * W + j] = color;
            }
        } 
        MPI_Send(buf, localSize, MPI_INT, 0, 1, MPI_COMM_WORLD);
        delete[] buf;
    }
    
    // for(int j = 0; j < image.rows; j ++) {
    //     for(int i = 0; i < image.cols; i++) {
    //         cv::Vec3b & color = image.at<cv::Vec3b>(j,i);
    //         double x0 = 3.0 * i / image.cols - 2;
    //         double y0 = 2.0 * j / image.rows - 1;
    //         double x = x0;
    //         double y = y0;
    //         bool good = true;
    //         for(int k = 0; k < n; k++) {
    //             double z = x * x + y * y;
    //             if(z > 4) {
    //                 good = false;
    //                 break;
    //             }
    //             double tmp = x;
    //             x = x * x - y * y + x0;
    //             y = 2 * tmp * y + y0;
    //         }
    //         if(good) {
    //             color[0] = 0;
    //             color[1] = 0;
    //             color[2] = 0;
    //         }
    //     }
    // }


    MPI_Finalize();
    return 0;
}
