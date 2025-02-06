#include "GstreamerApp.hpp"

void on_pad_added(GstElement *element, GstPad *pad, gpointer data)
{
    GstElement *sink = GST_ELEMENT(data);
    GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");

    // Lier le pad du décodeur au sink
    if (gst_pad_is_linked(sink_pad) == FALSE) {
        gst_pad_link(pad, sink_pad);
    }
    gst_object_unref(sink_pad);
}

GstreamerApp::GstreamerApp()
{
	this->pipeline = gst_pipeline_new("pipeline");
	this->source = gst_element_factory_make("v4l2src", "source");
	this->decoder = gst_element_factory_make("decodebin", "decoder");
	this->sink = gst_element_factory_make("autovideosink", "sink");

	if (!pipeline || !source || !decoder || !sink)
		throw ft_exception("Not all elements could be created.");

	gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
	if (!gst_element_link_many(source, decoder, NULL))
		throw ft_exception("Elements could not be linked.");
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), sink);
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

GstreamerApp::~GstreamerApp()
{
	std::cerr << "Deconstructor was called" << std::endl;
	gst_object_unref(this->pipeline);
}

void printHelp(void)
{
	std::cout << "There is a list of commands:" << std::endl;
	std::cout << "HELP" << std::endl;
	std::cout << "STATE [START | PAUSE | STOP]" << std::endl;
}

void GstreamerApp::setZoom(float zoom)
{
	g_object_set(this->cropFilter,
            "top",      (100 / zoom),
            "bottom",   (100 / zoom),
            "left",     (100 / zoom),
            "right",    (100 / zoom),
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
				std::cout << "Please use like this: STATE [START | PAUSE | STOP]" << std::endl;
		}
		else if (input[0] == "ZOOM")
		{
			this->setZoom(2);
		}
	}
}

void GstreamerApp::setPipelineState(GstState state)
{
	if (gst_element_set_state(this->pipeline, state) == GST_STATE_CHANGE_FAILURE)
		throw ft_exception("Unable to change the pipeline state.");
}