#include <gst/gst.h>
#include <unistd.h> // for sleep function
#include <iostream>

using namespace std;

int main (int argc, char *argv[])
{
    GstElement *pipeline;
    // *bin, *sink, *source;
    GstBus *bus;
    GstMessage *msg;
    
    const char* context = "gstlaunch v udpsrc port=5000 caps=\"application/xrtp\" ! rtph264depay ! ffdec_h264 ! ffmpegcolorspace ! ximagesink sync=false";
    const char* local_context = "gst-launch-1.0 -v playbin uri=file:///home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi";
    const char* test_context = "gstlaunch playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm";
    const char* uri_context = "gst-launch-1.0 uridecodebin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm ! videoconvert ! autovideosink";
    const char* flir_context = "udpsrc port=5000 caps=application/x-rtp,media=video,encoding-name=H264 ! rtph264depay ! h264parse ! nvv4l2decoder ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";
    const char* stackoverflow_context = "uridecodebin uri=file:///home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi ! nvvidconv ! video/x-raw(memory:NVMM) ! nvvidconv ! video/x-raw,format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1";
    
    // Initialize gstreamer
    gst_init (&argc, &argv);

    // Create C pipeline from terminal command (context)
    pipeline = gst_parse_launch(uri_context, NULL);
    // GstElement *appsink = pipeline.get_by_name("sink");
    // bin = gst_bin_new("my_bin");
    // source = gst_element_factory_make ("fakesrc", "source");
    // sink = gst_element_factory_make ("fakesink", "sink");
    // gst_bin_add_many(GST_BIN (bin), sink, NULL);
    // gst_bin_add (GST_BIN (pipeline), bin);
    
    // Start the pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    // sleep(1);
    // gst_element_set_state (pipeline, GST_STATE_PAUSED);
    // sleep(1);
    // gst_element_set_state (pipeline, GST_STATE_PLAYING);
    // Wait until error or EOS
    bus = gst_element_get_bus (pipeline);

    gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Free resources */

    if (msg != NULL)

        gst_message_unref (msg);
        // g_print(msg);

    gst_object_unref (bus);

    gst_element_set_state (pipeline, GST_STATE_NULL);

    gst_object_unref (pipeline);
}

