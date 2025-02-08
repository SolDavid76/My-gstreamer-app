#include "GstreamerApp.hpp"

void on_pad_added(GstElement *element, GstPad *pad, gpointer data)
{
	(void)element;
	GstElement *sink = GST_ELEMENT(data);
	GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");

	if (gst_pad_is_linked(sink_pad) == FALSE)
		gst_pad_link(pad, sink_pad);
	gst_object_unref(sink_pad);
}

void GstreamerApp::setSource(int ac, char **av)
{
	if (ac == 2 && std::string(av[1]) == "webcam")
	{
		this->source = gst_element_factory_make("v4l2src", "source");
		g_object_set(source, "device", "/dev/video0", NULL);
	}
	else if (ac == 2)
	{
		this->source = gst_element_factory_make("filesrc", "source");
		g_object_set(source, "location", av[1], NULL);
	}
	else
		throw ft_exception("Usage: ./a.out [webcam | <path-to-file>]");
}

gboolean handleBus(GstBus *bus, GstMessage *msg, gpointer user_data)
{
    (void)user_data;
    while ((msg = gst_bus_pop(bus)) != nullptr)
	{
        GError *error;
        gchar *debug_info;

		gst_message_parse_error(msg, &error, &debug_info);
		std::cerr << "Error: " << error->message << std::endl;
		std::cerr << "Debug Info: " << debug_info << std::endl;
		g_clear_error(&error);
		g_free(debug_info);
        gst_message_unref(msg);
    }
	return (true);
}


GstreamerApp::GstreamerApp(int ac, char **av)
{
	this->width = 1280;
	this->height = 720;
	this->pipeline = gst_pipeline_new("pipeline");
	this->setSource(ac, av);
	this->decoder = gst_element_factory_make("decodebin", "decoder");
	this->crop = gst_element_factory_make("videocrop", "crop");
	this->scale = gst_element_factory_make("videoscale", "scale");
	this->textoverlay = gst_element_factory_make("textoverlay", "textoverlay");
	this->capsfilter = gst_element_factory_make("capsfilter", "caps");
	this->sink = gst_element_factory_make("autovideosink", "sink");
	this->bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, (GstBusFunc)handleBus, nullptr);

	if (!pipeline || !source || !decoder || !crop || !scale || !textoverlay || !capsfilter || !sink)
		throw ft_exception("Not all elements could be created.");

	GstCaps *caps = gst_caps_new_simple("video/x-raw",
		"width", G_TYPE_INT, this->width,
		"height", G_TYPE_INT, this->height,
		NULL);

	g_object_set(capsfilter, "caps", caps, NULL);
	gst_caps_unref(caps);

	gst_bin_add_many(GST_BIN(pipeline), source, decoder, crop, scale, textoverlay, capsfilter, sink, NULL);
	if (!gst_element_link(source, decoder) || !gst_element_link_many(crop, scale, textoverlay, capsfilter, sink, NULL))
		throw ft_exception("Elements could not be linked.");
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), crop);
}

GstreamerApp::GstreamerApp(GstreamerApp const& src)
{
	*this = src;
}

GstreamerApp& GstreamerApp::operator=(GstreamerApp const& src)
{
	if (this != &src)
	{
		// this->pipeline = src.pipeline;
		// this->source = src.source;
		// this->filter = src.filter;
		// this->sink = src.sink;
		// this->bus = src.bus;
	}
	return (*this);
}

void ft_gst_object_unref(GstElement	*element)
{
	if (element)
		gst_object_unref(element);
}

GstreamerApp::~GstreamerApp()
{
	std::cout << "Quitting..." << std::endl;
	this->setPipelineState(GST_STATE_NULL);
	ft_gst_object_unref(this->pipeline);
	ft_gst_object_unref(this->source);
	ft_gst_object_unref(this->decoder);
	ft_gst_object_unref(this->crop);
	ft_gst_object_unref(this->scale);
	ft_gst_object_unref(this->textoverlay);
	ft_gst_object_unref(this->capsfilter);
	ft_gst_object_unref(this->sink);
	if (this->bus)
		gst_object_unref(this->bus);
}

void printHelp(void)
{
	std::cout << "There is a list of commands:" << std::endl;
	std::cout << "HELP" << std::endl;
	std::cout << "STATE [START | PAUSE | STOP]" << std::endl;
	std::cout << "ZOOM [1-5]" << std::endl;
	std::cout << "TEXT <text> [TOP | CENTER | BOTTOM] [LEFT | CENTER | RIGHT]" << std::endl;
	std::cout << "QUIT" << std::endl;
}

void GstreamerApp::setZoom(int zoomLevel)
{
	if (zoomLevel < 1 || 5 < zoomLevel)
	{
		std::cerr << "Zoom value need to be [1-5]" << std::endl;
		return;
	}
	int pixelxcut = (this->width - (this->width / zoomLevel)) / 2;
	int pixelycut = (this->height - (this->height / zoomLevel)) / 2;
	g_object_set(crop,
		"top", pixelycut,
		"bottom", pixelycut,
		"left", pixelxcut,
		"right", pixelxcut,
		NULL);
}

void GstreamerApp::handleUser(void)
{
	printHelp();

	while (42)
	{
		std::vector<std::string> input = ft_split(ft_getline());
		if (input[0] == "HELP")
			printHelp();
		else if (input[0] == "STATE")
		{
			if (input[1] == "START")
				this->setPipelineState(GST_STATE_PLAYING);
			else if (input[1] == "PAUSE")
				this->setPipelineState(GST_STATE_PAUSED);
			else if (input[1] == "STOP")
				this->setPipelineState(GST_STATE_NULL);
			else
				std::cerr << "Please use like this: STATE [START | PAUSE | STOP]" << std::endl;
		}
		else if (input[0] == "ZOOM")
			this->setZoom(std::stoi(input[1]));
		else if (input[0] == "TEXT")
			g_object_set(textoverlay, "text", input[1].c_str(), NULL);
		else if (input[0] == "QUIT")
			break;
	}
}

void GstreamerApp::setPipelineState(GstState state)
{
	if (gst_element_set_state(this->pipeline, state) == GST_STATE_CHANGE_FAILURE)
		throw ft_exception("Unable to change the pipeline state.");
}
