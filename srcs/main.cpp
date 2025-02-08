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
		throw ft_exception("Bus Error");
	}
	return (true);
}

int main(int ac, char **av)
{
	try
	{
		GstreamerApp app(ac, av);

		app.setPipelineState(GST_STATE_PLAYING);

		app.handleUser();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
