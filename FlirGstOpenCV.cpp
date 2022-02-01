#include <gst/gst.h>
#include <unistd.h> // for sleep function
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/* To debug export environment variable: export GST_DEBUG="*:6" */

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
    
    int fail = 0;

    while(!cap.isOpened())
    {
        cout<<"VideoCapture not opened"<<endl;
        return false;
    }

    Mat frame, canny, hough, houghP;
    vector<Vec2f> lines;
    vector<Vec4i> linesP;
    float rho, theta;
    Point pt1, pt2;
    double a, b, x0, y0;
    while(true) {

        cap.read(frame);

        if(frame.empty())
            return true;

        Canny(frame, canny, 50, 200, 3);

        cvtColor(canny, hough, COLOR_GRAY2BGR);
        houghP = hough.clone();

        HoughLines(canny, lines, 1, CV_PI/180, 150, 50, 10);
        for(size_t i = 0;i < lines.size(); i++)
        {
            rho = lines[i][0], theta = lines[i][1];
            pt1, pt2;
            a = cos(theta), b = sin(theta);
            x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a)); 
            line(hough, pt1, pt2, Scalar(0,0,255), 1, LINE_AA);
        }

        HoughLinesP(canny, linesP, 1, CV_PI/180, 50, 50, 20 );
        for( size_t i = 0; i < linesP.size(); i++ )
        {
            Vec4i l = linesP[i];
            line( houghP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 1, LINE_AA);
        }

        imshow("Receiver", frame);
        imshow("Canny output", canny);
        imshow("Hough's transform", hough);
        imshow("Hough's probabilistic transform", houghP);

        if(waitKey(1) == 'r')
            return false;
    }
    destroyWindow("Receiver");
    return true;
}

int main(int argc, char *argv[])
{
    const char* CAPS = "video/x-raw,format=RGB,width=160,pixel-aspect-ratio=1/1";

    GstElement *pipeline;

    // TESTED PIPELINES
    const char* uri_CV_context = "uridecodebin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm ! nvvidconv \
    ! video/x-raw(memory:NVMM) ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";
    
    // Camera PIPELINE
    const char* IRnet = "gstlaunch v udpsrc port=5000 caps=\"application/xrtp\" ! rtph264depay ! ffdec_h264 ! ffmpegcolorspace ! ximagesink sync=false";
    const char* IRnet_opencv = "udpsrc port=5000 caps=application/x-rtp,media=video,encoding-name=H264 ! rtph264depay ! h264parse ! nvv4l2decoder ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";
    // URI PIPELINE
    const char* uri_context = "uridecodebin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm ! videoconvert ! videoscale ! appsink name=sink ! caps=video/x-raw,format=RGB,width=160,pixel-aspect-ratio=1/1";
    const char* non_nvvidconv_context = "uridecodebin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm ! nvvidconv ! video/x-raw(memory:NVMM) ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! videoscale ! appsink drop=1 ! caps=video/x-raw,format=BGR,width=160,pixel-aspect-ratio=1/1";
    // File PIPELINE
    // gst-launch-1.0 filesrc location=test.avi ! avidemux name=demux  demux.audio_00 ! decodebin ! audioconvert ! audioresample ! autoaudiosink   demux.video_00 ! queue ! decodebin ! videoconvert ! videoscale ! autovideosink
    const char* filesource = "filesrc location=///home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi ! nvvidconv ! video/x-raw(memory:NVMM) ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";
    const char* stackoverflow_context2 = "uridecodebin uri=file:///home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";
    const char* stackoverflow_context = "uridecodebin uri=file:///home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi ! nvvidconv ! video/x-raw(memory:NVMM) ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";
    

    // gst_init(&argc, &argv);
    // pipeline = gst_parse_launch(test_context, NULL);
    bool correct_execution = receiver(filesource);
    if(correct_execution){
        cout << "openCV - gstreamer works!" << endl;
    } else {
        cout << "openCV - gstreamer FAILED" << endl;
    }
}
