#include "opencv2/opencv.hpp"
using namespace cv;

#include <iostream>
using namespace std;

#include <opencv2/imgproc.hpp>

#include <chrono>

int main()
{
    VideoCapture cap;
    cap.open(0);
    if (!cap.isOpened()) {
        cout << "could not open the VideoCapture !" << endl;
        return -1;
    }
    int fps = 0;
    int filter = 0;

    auto startFps = chrono::high_resolution_clock::now();
    Mat frame;
    Mat frame1;
    while (1) {
        auto startInput = chrono::high_resolution_clock::now();
        bool ok = cap.read(frame);
        if (!ok)
            break;
        //cap.read(frame1);
        auto finishInput = chrono::high_resolution_clock::now();

        auto startTransform = chrono::high_resolution_clock::now();
        if (filter == 1) {
            frame = frame * 2;
            for (int i = 0; i < frame.rows; i++)
                for (int j = 0; j < frame.cols; j++)
                    if (frame.at<Vec3b>(i, j)[0] < frame.at<Vec3b>(i, j)[1] - 10 &&
                        frame.at<Vec3b>(i, j)[2] < frame.at<Vec3b>(i, j)[1] - 10 &&
                        frame.at<Vec3b>(i, j)[1] > 64)
                    {
                        frame.at<Vec3b>(i, j)[0] = 0;
                        frame.at<Vec3b>(i, j)[1] = 0;
                        frame.at<Vec3b>(i, j)[2] = 255;
                    }
        }
        if (filter == 2) {
            //Mat frame2;
            cvtColor(frame, frame, COLOR_BGR2HSV);
        }
        auto finishTransform = chrono::high_resolution_clock::now();

        auto startOutput = chrono::high_resolution_clock::now();
        imshow("camera", frame);
        //imshow("filter2", frame2);
        auto finishOutput = chrono::high_resolution_clock::now();

        auto finishFps = chrono::high_resolution_clock::now();
        auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(finishFps - startFps);

        
        int k = waitKey(33);
        if (k == 27) // выход на хот кейс ESC
            break;


        if (int_ms.count() >= 1000) {
            std::chrono::duration<double, std::milli> input = finishInput - startInput;
            std::chrono::duration<double, std::milli> transform = finishTransform - startTransform;
            std::chrono::duration<double, std::milli> output = finishOutput - startOutput;
            cout << "FPS: " << fps
                << ". Input time: " << input.count() << "ms"
                << ". transform time: " << transform.count() << "ms"
                << ". Output time: " << output.count() << " ms" << endl;
            startFps = chrono::high_resolution_clock::now();
            fps = 0;
        }
        fps++;
    }

    destroyWindow("camera");

    return 0;
}