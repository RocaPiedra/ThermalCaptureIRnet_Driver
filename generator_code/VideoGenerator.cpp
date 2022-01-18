#include <stdio.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <fstream>
#include <string>
#include <memory>
#include <iostream>
#include <list>
#include <numeric>
#include <dirent.h>
#include <string.h>
#include <chrono>
#include <unistd.h>

#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int VideoGenerator(const char *THM_DATA_PATH, string filename, bool show)
{
    /** Path of the data set to simulate image acquisition from a camera stream */
    // const char *THM_DATA_PATH = "/home/nvidia/repos/APPIDE/vidtest/THERMAL/IMAGES/subset/";
    DIR *dir = opendir(THM_DATA_PATH);
    struct dirent *entry;

    char *imageName;
    string input_name;
    // bool show = true;
    Mat image;
    int counter = 0;


    string videoName = "thermal_video_ordered";
    string extension = ".avi";
    Mat req_image = imread("/home/nvidia/repos/APPIDE/vidtest/RGB/IMAGES/1024x1024/DSC02397.JPG");
    bool isColor = (req_image.type() == CV_8UC3);
    cout << "image size is " << req_image.size() << endl;

    VideoWriter writer;
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');  // select desired codec (must be available at runtime)
    double fps = 30;                          // framerate of the created video stream
    // string filename = "/home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi";             // name of the output video file
    writer.open(filename, codec, fps, req_image.size(), isColor);
    
    if ( !writer.isOpened())
    {
        cout  << "Could not open the input video." << THM_DATA_PATH << endl;
        return -1;
    }
    cout << "Writing videofile: " << filename << endl;
    if(show){namedWindow("Live_writer",cv::WINDOW_AUTOSIZE);}


    vector<string> path_vector; //vector storing all paths to sort
    
    while ((entry = readdir(dir)) != NULL)
    {
        imageName = entry->d_name;

        // Check image name to avoid '.' and '..' entries
        if (strcmp(imageName, ".") != 0 && strcmp(imageName, "..") != 0)
        {
            input_name = string(THM_DATA_PATH) + string(imageName);
            cout << "Image to write: " << input_name << endl;
            path_vector.push_back(input_name);
            counter++;
        }
        if(show){destroyAllWindows();}    
    }
    sort(path_vector.begin(), path_vector.end());

    for(auto i : path_vector)    {
        cout << "path is "<< i << endl;
        image = imread(i);
        if(show){imshow("Live_writer",image);}
        if(show){waitKey(300);}
        writer.write(image);
    }
    
    cout << "A total of " << counter << " images have been written in " << filename << endl;
    return 0;
}

int main(void)
{
    // const char* input_path = "/home/nvidia/repos/APPIDE/vidtest/THERMAL/IMAGES/subset/";
    // string output_path = "/home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi";
    const char* input_path = "/home/nvidia/repos/APPIDE/vidtest/RGB/IMAGES/1024x1024/";
    string output_path = "/home/nvidia/repos/APPIDE/vidtest/RGB/opticalVideo.avi";
    
    bool visualize = false;
    VideoGenerator(input_path, output_path, visualize);

    return 0;
}