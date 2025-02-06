#include <gst/gst.h>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <vector>

std::string ft_getline(void)
{
	std::string res;

	if (std::getline(std::cin, res))
		return (res);
	else
		return ("");
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
	GstElement *sink = GST_ELEMENT(data);
	GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");

	if (gst_pad_is_linked(sink_pad) == FALSE) {
		gst_pad_link(pad, sink_pad);
	}
	gst_object_unref(sink_pad);
}

int main(int ac, char **av)
{
	gst_init(&ac, &av);

	GstElement *pipeline = gst_pipeline_new("pipeline");
	GstElement *source = gst_element_factory_make("v4l2src", "source");
	// GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
    GstElement *convert = gst_element_factory_make("videoconvert", "convert");
	GstElement *crop = gst_element_factory_make("videocrop", "crop");
	GstElement *scale = gst_element_factory_make("videoscale", "scale");
	GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps");
	GstElement *sink = gst_element_factory_make("autovideosink", "sink");

	if (!pipeline || !source || !convert || !crop || !scale || !capsfilter || !sink)
		return (1);

	g_object_set(crop,
		"top", 0,
		"bottom", 0,
		"left", 0,
		"right", 0,
		NULL);
		
	
	GstCaps *caps = gst_caps_new_simple("video/x-raw",
		"width", G_TYPE_INT, 1920,
		"height", G_TYPE_INT, 1080,
		NULL);

	g_object_set(capsfilter, "caps", caps, NULL);
	gst_caps_unref(caps);

	gst_bin_add_many(GST_BIN(pipeline), source, convert, crop, scale, capsfilter, sink, NULL);
	if (!gst_element_link_many(source, convert, crop, scale, capsfilter, sink, NULL))
		return (1);
	// g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), sink);

	if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
		return (1);

	while (42)
	{
		std::vector<std::string> input = ft_split(ft_getline());
		if (input[0] == "HELP")
			printHelp();
		else if (input[0] == "STATE")
		{
			if (input[1] == "START")
				gst_element_set_state(pipeline, GST_STATE_PLAYING);
			else if (input[1] == "PAUSE")
				gst_element_set_state(pipeline, GST_STATE_PAUSED);
			else if (input[1] == "STOP")
				gst_element_set_state(pipeline, GST_STATE_NULL);
			else
				std::cout << "Please use like this: STATE [START | PAUSE | STOP]" << std::endl;
		}
		else if (input[0] == "ZOOM")
		{
			int zoom = std::stoi(input[1]);
			std::cout << "try: " << zoom << std::endl;
			g_object_set(crop,
				"top", zoom,     // Augmente le crop en haut
				"bottom", zoom,  // Augmente le crop en bas
				"left", zoom,   // Augmente le crop à gauche
				"right", zoom,  // Augmente le crop à droite
				NULL);
		}
	}
}