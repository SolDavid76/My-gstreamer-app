#include <gst/gst.h>
#include <iostream>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GstElement *pipeline = gst_parse_launch("autovideosrc ! videoconvert ! autovideosink", NULL);
    // GstElement *pipeline = gst_parse_launch("autovideosrc ! videoconvert ! ximagesink", NULL);

    if (!pipeline) {
        std::cerr << "Error: pipeline wont create" << std::endl;
        return -1;
    }

    // Lancer le pipeline
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Error: pipeline wont start" << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    std::cout << "Pipeline us running..." << std::endl;

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
