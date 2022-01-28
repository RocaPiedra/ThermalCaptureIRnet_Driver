#include <gst/gst.h>
#include <unistd.h> // for sleep function
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void sender()
{
    // VideoCapture: Getting frames using 'v4l2src' plugin, format is 'BGR' because
    // the VideoWriter class expects a 3 channel image since we are sending colored images.
    // Both 'YUY2' and 'I420' are single channel images. 
    VideoCapture cap("v4l2src ! video/x-raw,format=BGR,width=640,height=480,framerate=30/1 ! appsink",CAP_GSTREAMER);

    // VideoWriter: 'videoconvert' converts the 'BGR' images into 'YUY2' raw frames to be fed to
    // 'jpegenc' encoder since 'jpegenc' does not accept 'BGR' images. The 'videoconvert' is not
    // in the original pipeline, because in there we are reading frames in 'YUY2' format from 'v4l2src'
    VideoWriter out("appsrc ! videoconvert ! video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=5000",CAP_GSTREAMER,0,30,Size(640,480),true);

    if(!cap.isOpened() || !out.isOpened())
    {
        cout<<"VideoCapture or VideoWriter not opened"<<endl;
        exit(-1);
    }

    Mat frame;

    while(true) {

        cap.read(frame);

        if(frame.empty())
            break;

        out.write(frame);

        imshow("Sender", frame);
        if(waitKey(1) == 's')
            break;
    }
    destroyWindow("Sender");
}

bool receiver(const char* context)
{    
    // The sink caps for the 'rtpjpegdepay' need to match the src caps of the 'rtpjpegpay' of the sender pipeline
    // Added 'videoconvert' at the end to convert the images into proper format for appsink, without
    // 'videoconvert' the receiver will not read the frames, even though 'videoconvert' is not present
    // in the original working pipeline
    VideoCapture cap(context, CAP_GSTREAMER);

    while(!cap.isOpened())
    {
        cout<<"VideoCapture not opened"<<endl;
        return false;
    }

    Mat frame;

    while(true) {

        cap.read(frame);

        if(frame.empty())
            return true;

        imshow("Receiver", frame);
        if(waitKey(1) == 'r')
            return false;
    }
    destroyWindow("Receiver");
    return true;
}

int main(int argc, char *argv[])
{
    GstElement *pipeline;
    const char* context = "uridecodebin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm ! nvvidconv \
    ! video/x-raw(memory:NVMM) ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";

    bool correct_execution = receiver(context);

    if(correct_execution){
        cout << "openCV - gstreamer works!" << endl;
    } else {
        cout << "openCV - gstreamer FAILED" << endl;
    }
}