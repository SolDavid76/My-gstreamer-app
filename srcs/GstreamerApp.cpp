#include "GstreamerApp.hpp"

GstreamerApp::GstreamerApp(int ac, char **av)
{
	gst_init(&ac, &av);

	this->width = 1280;
	this->height = 720;
	this->pipeline = gst_pipeline_new("pipeline");
	this->setSource(ac, av);
	this->decoder = gst_element_factory_make("decodebin", "decoder");
	this->cropFilter = gst_element_factory_make("videocrop", "cropFilter");
	this->scaleFilter = gst_element_factory_make("videoscale", "scaleFilter");
	this->textoverlay = gst_element_factory_make("textoverlay", "textoverlay");
	this->capsfilter = gst_element_factory_make("capsfilter", "capsFilter");
	this->sink = gst_element_factory_make("autovideosink", "sink");
	this->bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, (GstBusFunc)handleBus, nullptr);

	if (!pipeline || !source || !decoder || !cropFilter || !scaleFilter || !textoverlay || !capsfilter || !sink)
		throw ft_exception("Not all elements could be created.");

	GstCaps *caps = gst_caps_new_simple("video/x-raw",
		"width", G_TYPE_INT, this->width,
		"height", G_TYPE_INT, this->height,
		NULL);

	g_object_set(capsfilter, "caps", caps, NULL);
	gst_caps_unref(caps);

	gst_bin_add_many(GST_BIN(pipeline), source, decoder, cropFilter, scaleFilter, textoverlay, capsfilter, sink, NULL);
	if (!gst_element_link(source, decoder) || !gst_element_link_many(cropFilter, scaleFilter, textoverlay, capsfilter, sink, NULL))
		throw ft_exception("Elements could not be linked.");
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), cropFilter);
}

GstreamerApp::GstreamerApp(GstreamerApp const& src)
{
	*this = src;
}

GstreamerApp& GstreamerApp::operator=(GstreamerApp const& src)
{
	if (this != &src)
	{
		this->width = src.width;
		this->height = src.height;
		this->pipeline = src.pipeline;
		this->source = src.source;
		this->decoder = src.decoder;
		this->cropFilter = src.cropFilter;
		this->scaleFilter = src.scaleFilter;
		this->textoverlay = src.textoverlay;
		this->capsfilter = src.capsfilter;
		this->sink = src.sink;
		this->bus = src.bus;
	}
	return (*this);
}

GstreamerApp::~GstreamerApp()
{
	std::cout << "Quitting..." << std::endl;
	gst_element_set_state(pipeline,GST_STATE_NULL);
	if (this->pipeline)
		gst_object_unref(this->pipeline);
	if (this->bus)
		gst_object_unref(this->bus);
	gst_deinit();
}

void GstreamerApp::setSource(int ac, char **av)
{
	if (ac == 2 && std::string(av[1]) == "webcam")
	{
		this->source = gst_element_factory_make("v4l2src", "source");
		g_object_set(source, "device", "/dev/video0", NULL);
	}
	else if (ac == 2 && checkFile(av[1]))
	{
		this->source = gst_element_factory_make("filesrc", "source");
		g_object_set(source, "location", av[1], NULL);
	}
	else
		throw ft_exception("Usage: ./a.out [webcam | <path-to-file>]");
}

void GstreamerApp::printHelp()
{
	std::cout << "There is a list of commands:" << std::endl;
	std::cout << "HELP" << std::endl;
	std::cout << "STATE [START | PAUSE | STOP]" << std::endl;
	std::cout << "ZOOM [1-5]" << std::endl;
	std::cout << "TEXT <text>" << std::endl;
	std::cout << "QUIT" << std::endl;
}

int GstreamerApp::setZoom(int zoomLevel)
{
	if (zoomLevel < 1 || 5 < zoomLevel)
		return (1);
	int pixelxcut = (this->width - (this->width / zoomLevel)) / 2;
	int pixelycut = (this->height - (this->height / zoomLevel)) / 2;
	g_object_set(cropFilter,
		"top", pixelycut,
		"bottom", pixelycut,
		"left", pixelxcut,
		"right", pixelxcut,
		NULL);
	std::cout << "Set zoom to: x" << zoomLevel << std::endl;

	return (0);
}

void GstreamerApp::handleUser()
{
	this->printHelp();

	while (42)
	{
		std::vector<std::string> input = ft_split(ft_getline());
		if (input[0] == "HELP")
			this->printHelp();
		else if (input[0] == "STATE")
		{
			if (input.size() == 2 && input[1] == "START")
				this->setPipelineState(GST_STATE_PLAYING);
			else if (input.size() == 2 && input[1] == "PAUSE")
				this->setPipelineState(GST_STATE_PAUSED);
			else if (input.size() == 2 && input[1] == "STOP")
				this->setPipelineState(GST_STATE_NULL);
			else
				std::cerr << "Please use like this: STATE [START | PAUSE | STOP]" << std::endl;
		}
		else if (input[0] == "ZOOM")
		{
			if (input.size() != 2 || this->setZoom(ft_stoi(input[1])))
				std::cerr << "Please use like this: ZOOM [1-5]" << std::endl;
		}
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
	else if (state == GST_STATE_PLAYING)
		std::cout << "Pipeline is running..." << std::endl;
	else if (state == GST_STATE_PAUSED)
		std::cout << "Pipeline is paused..." << std::endl;
	else if (state == GST_STATE_NULL)
		std::cout << "Pipeline is stopped..." << std::endl;
}
