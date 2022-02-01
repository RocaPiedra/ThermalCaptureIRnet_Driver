#include <unistd.h> // for sleep function
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(void)
{
    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;
    auto start = Time::now();
    auto end = Time::now();
    
    int width = 640;
    int height = 512;
    float fps = 8.33;
    width = 1920;
    height = 1080;
    fps = 30;
    int frame_ms = 1000/fps;
    ms frame_time = ms {frame_ms};
    int duration = 60; // seconds
    int num_images = int(ceil(duration*fps));

    string path = "/home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi";
    
    string text;
    fsec time_to_sleep, elapsed, text_in_chrono;
    float text_in_number;
    auto init = Time::now();

    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');  // select desired codec (must be available at runtime)
    Mat background(height, width, CV_8UC3, Scalar(0));
    bool isColor = (background.type() == CV_8UC3);
    string filename = "/home/nvidia/repos/APPIDE/flir/generator_code/syncro_tests/fullhd_30fps.avi";
    VideoWriter writer(filename, codec, fps, (width, height), isColor);

    if ( !writer.isOpened())
    {
        cout  << "Could not open the video writer." << endl;
        return -1;
    }

    cout << "Writing videofile: " << filename << endl;

    for (int i=0;i<=num_images;i++)
    {   

        start = Time::now();
        text_in_chrono = start - init;
        text_in_number = text_in_chrono.count();
        text = to_string(text_in_number);
        Mat background(height, width, CV_8UC3, Scalar(0));
        putText(background, //target image
                    text, //text
                    Point(background.cols / 3, background.rows / 2),
                    FONT_HERSHEY_DUPLEX,
                    1.0,
                    CV_RGB(255, 255, 255), //font color
                    2);
        
        imshow("Time", background);
        writer.write(background);
        waitKey(1);
        end = Time::now();
        elapsed = end - start;
        time_to_sleep = frame_time - elapsed;
        // cout << "start: " << start.count() << "s, end:" << end.count() << "s" <<endl;
        cout << "elapsed time is: " << elapsed.count() << "s, sleep for:" << time_to_sleep.count() << "s" <<endl;
        std::this_thread::sleep_for(time_to_sleep);
    }
    writer.release();
    destroyAllWindows();
    return 0;    
}