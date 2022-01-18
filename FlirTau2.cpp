#include <gst/gst.h>
#include <unistd.h> // for sleep function
#include <iostream>

using namespace std;

int main (int argc, char *argv[])
{
    GstElement *pipeline, *bin, *sink, *source;
    GstBus *bus;
    GstMessage *msg;
    
    const char* context = "gstlaunch v udpsrc port=5000 caps=\"application/xrtp\" ! rtph264depay ! ffdec_h264 ! ffmpegcolorspace ! ximagesink sync=false";
    const char* local_context = "playbin uri=file:///home/nvidia/repos/APPIDE/vidtest/THERMAL/thermalVideo.avi";
    const char* test_context = "gstlaunch playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm ! video/x-raw,width=1280,height=800,format=(string)GRAY8 ! videoconvert ! videoscale ! video/x-raw,width=640,height=400,format=BGR ! appsink";
    // Initialize gstreamer
    gst_init (&argc, &argv);

    // Create C pipeline from terminal command (context)
    pipeline = gst_parse_launch(local_context, NULL);
    // GstElement *appsink = pipeline.get_by_name("sink");
    bin = gst_bin_new("my_bin");
    source = gst_element_factory_make ("fakesrc", "source");
    sink = gst_element_factory_make ("fakesink", "sink");
    gst_bin_add_many(GST_BIN (bin), sink, NULL);
    gst_bin_add (GST_BIN (pipeline), bin);
    
    // Start the pipeline
    gst_element_set_state(bin, GST_STATE_PLAYING);
    // sleep(1);
    // gst_element_set_state (pipeline, GST_STATE_PAUSED);
    // sleep(1);
    // gst_element_set_state (pipeline, GST_STATE_PLAYING);
    // Wait until error or EOS
    bus = gst_element_get_bus (bin);

    gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Free resources */

    if (msg != NULL)

        gst_message_unref (msg);
        // g_print(msg);

    gst_object_unref (bus);

    gst_element_set_state (pipeline, GST_STATE_NULL);

    gst_object_unref (pipeline);
}

