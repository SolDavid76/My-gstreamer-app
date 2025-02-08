#include "./includes/GstreamerApp.hpp"

std::string ft_getline(void)
{
	std::string res;

	if (std::getline(std::cin, res))
		return (res);
	else
		throw ft_exception("ctrl+D");
}

std::vector<std::string> ft_split(std::string input)
{
    std::vector<std::string> res;
    std::istringstream iss(input);
    std::string word;

    while (iss >> word)
	{
        res.push_back(word);
    }

    return (res);
}

void printHelp(void)
{
	std::cout << "There is a list of commands:" << std::endl;
	std::cout << "HELP" << std::endl;
	std::cout << "STATE [START | PAUSE | STOP]" << std::endl;
}

void on_pad_added(GstElement *element, GstPad *pad, gpointer data)
{
	(void)element;
    GstElement *sink = GST_ELEMENT(data);
    GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");

    if (gst_pad_is_linked(sink_pad) == FALSE) {
        gst_pad_link(pad, sink_pad);
    }
    gst_object_unref(sink_pad);
}


int main(int ac, char **av)
{
    try
    {
        gst_init(&ac, &av);

        int width = 1280;
        int height = 720;

        GstElement *pipeline = gst_pipeline_new("pipeline");
        GstElement *source = gst_element_factory_make("v4l2src", "source");
        GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
        GstElement *crop = gst_element_factory_make("videocrop", "crop");
        GstElement *scale = gst_element_factory_make("videoscale", "scale");
        GstElement *textoverlay = gst_element_factory_make("textoverlay", "textoverlay");
        GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps");
        GstElement *sink = gst_element_factory_make("xvimagesink", "sink");

        if (!pipeline || !source || !decoder || !crop || !scale || !textoverlay || !capsfilter || !sink)
            throw ft_exception("Not all elements could be created.");

        GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                           "width", G_TYPE_INT, width,
                                           "height", G_TYPE_INT, height,
                                           NULL);

        g_object_set(capsfilter, "caps", caps, NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(GST_BIN(pipeline), source, decoder, crop, scale, textoverlay, capsfilter, sink, NULL);
        if (!gst_element_link(source, decoder) || !gst_element_link_many(crop, scale, textoverlay, capsfilter, sink, NULL))
            throw ft_exception("Elements could not be linked.");
        g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), crop);

        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        std::cout << "Pipeline is running..." << std::endl;

        while (42)
        {
            std::vector<std::string> input = ft_split(ft_getline());
            if (input[0] == "HELP")
                printHelp();

            // Vous pouvez ajouter d'autres conditions ici pour quitter proprement
            if (input[0] == "QUIT")
                break;
        }

        // Nettoyage avant de quitter
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        gst_object_unref(source);
        gst_object_unref(decoder);
        gst_object_unref(crop);
        gst_object_unref(scale);
        gst_object_unref(textoverlay);
        gst_object_unref(capsfilter);
        gst_object_unref(sink);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
