#include <omp.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <iostream>
#include <thread>

#define THREAD_AMOUNT 4

int main() {    
    cv::VideoCapture cap("samples/ZUA.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть видеофайл!" << std::endl;
        return -1;
    }

    cv::CascadeClassifier face_cascade;    
    if (!face_cascade.load("haarcascade_frontalface_default.xml")) {        
        std::cerr << "Ошибка: не удалось загрузить классификатор лиц!" << std::endl;        
        return -1;    
    }    

    cv::CascadeClassifier eye_cascade;    
    if (!eye_cascade.load("haarcascade_eye.xml")) {        
        std::cerr << "Ошибка: не удалось загрузить классификатор лиц!" << std::endl;        
        return -1;    
    }    

    cv::CascadeClassifier smile_cascade;    
    if (!smile_cascade.load("haarcascade_smile.xml")) {        
        std::cerr << "Ошибка: не удалось загрузить классификатор лиц!" << std::endl;        
        return -1;    
    }    

    cv::Mat img;

    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    cv::Size frameSize(frame_width, frame_height);
    cv::VideoWriter writer("output.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, frameSize, true);

    omp_set_num_threads(THREAD_AMOUNT);

    while(true) {
        cap >> img;
        if(img.empty()) {
            break;
        }

        cv::Mat gray;    
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);    
        cv::equalizeHist(gray, gray); // Улучшаем контрастность      

        std::vector<cv::Rect> faces;    
        std::vector<cv::Rect> faces2;    
        std::vector<cv::Rect> eyes;    
        std::vector<cv::Rect> smiles;    

// #pragma omp parallel 
// {
//     #pragma omp single
//     {
//             #pragma omp task
//             {
               face_cascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(50, 50));

//             }
//             #pragma omp task
//             {
                eye_cascade.detectMultiScale(gray, eyes, 1.2, 10, 0, cv::Size(50, 50));    
//             }
//             #pragma omp task
//             {
                smile_cascade.detectMultiScale(gray, smiles, 1.2, 50, 0, cv::Size(40, 20));    
//             }
//     }
// }

        // std::thread a([gray, &faces, &face_cascade] { face_cascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(50, 50)); });       
        // std::thread b([gray, &eyes, &eye_cascade] { eye_cascade.detectMultiScale(gray, eyes, 1.2, 10, 0, cv::Size(50, 50)); });       
        // std::thread c([gray, &smiles, &smile_cascade] { smile_cascade.detectMultiScale(gray, smiles, 1.2, 50, 0, cv::Size(40, 20)); });       
        //
        // a.join();
        // b.join();
        // c.join();

        for (size_t i = 0; i < faces.size(); i++) {  
            cv::rectangle(img, faces[i], cv::Scalar(255, 0, 0), 2);    
        }

        for (size_t i = 0; i < eyes.size(); i++) {  
            bool bad = true;
            cv::Point xy(eyes[i].x, eyes[i].y);
            for(auto face: faces) {
                if(face.contains(xy)){
                    bad = false;
                    break;
                }
            }
            if(!bad) {
                cv::rectangle(img, eyes[i], cv::Scalar(0, 0, 255), 2);    
            }
        }

        for (size_t i = 0; i < smiles.size(); i++) {  
            bool bad = true;
            cv::Point xy(smiles[i].x, smiles[i].y);
            for(auto face: faces) {
                if(face.contains(xy)){
                    bad = false;
                    break;
                }
            }
            if(!bad) {
                cv::rectangle(img, smiles[i], cv::Scalar(0, 255, 0), 2);    
            }
        }

        cv::imshow("Обнаруженные лица", img);    
        writer.write(img);

        if((cv::waitKey(30) & 0xEFFFFF) == 27) {
            break;
        }
    }

    writer.release();
    cap.release();
    cv::destroyAllWindows();


    return 0;
}
