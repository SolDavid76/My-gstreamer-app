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

void on_pad_added(GstElement *element, GstPad *pad, gpointer data)
{
    GstElement *sink = GST_ELEMENT(data);
    GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");
    if (gst_pad_is_linked(sink_pad) == FALSE)
    {
        gst_pad_link(pad, sink_pad);
    }
    gst_object_unref(sink_pad);
}

int main(int argc, char **argv)
{
    gst_init(&argc, &argv);

    // GstElement *pipeline = gst_pipeline_new("video-pipeline");
    // GstElement *source = gst_element_factory_make("v4l2src", "source");
    // GstElement *convert = gst_element_factory_make("videoconvert", "convert");
    // GstElement *crop = gst_element_factory_make("videocrop", "crop");
    // GstElement *videoscale = gst_element_factory_make("videoscale", "scaler");
    // GstElement *capsfilter = gst_element_factory_make("capsfilter", "filter");
    // GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    // if (!pipeline || !source || !videoscale || !sink)
    // {
    //     std::cerr << "Failed to create elements" << std::endl;
    //     return -1;
    // }

	// g_object_set(crop,
    //     "top", 0,     // Coupe 0 pixels en haut
    //     "bottom", 0,  // Coupe 0 pixels en bas
    //     "left", 0,   // Coupe 0 pixels à gauche
    //     "right", 0,  // Coupe 0 pixels à droite
    //     NULL);

	// GstCaps *caps = gst_caps_new_simple(
    //     "video/x-raw",
    //     "width", G_TYPE_INT, 640,  // Nouvelle largeur après zoom
    //     "height", G_TYPE_INT, 480, // Nouvelle hauteur après zoom
    //     NULL);
    // g_object_set(capsfilter, "caps", caps, NULL);
    // gst_caps_unref(caps);


    // gst_bin_add_many(GST_BIN(pipeline), source, videoscale, capsfilter, sink, NULL);
    // if (!gst_element_link_many(source, videoscale, capsfilter, sink, NULL))
    // {
    //     std::cerr << "Failed to link elements" << std::endl;
    //     return -1;
    // }
	GstElement *pipeline = gst_pipeline_new("video-zoom-pipeline");
    GstElement *source = gst_element_factory_make("v4l2src", "source");
    GstElement *crop = gst_element_factory_make("videocrop", "crop");
    GstElement *scale = gst_element_factory_make("videoscale", "scale");
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !crop || !scale || !capsfilter || !sink) { // || !convert
        g_printerr("Échec de la création des éléments.\n");
        return -1;
    }

    // 📌 Configuration initiale du crop
    g_object_set(crop,
        "top", 0,
        "bottom", 0,
        "left", 0,
        "right", 0,
        NULL);

    // 📌 Configuration du redimensionnement après crop
    GstCaps *caps = gst_caps_new_simple(
        "video/x-raw",
        "width", G_TYPE_INT, 640,
        "height", G_TYPE_INT, 480,
        NULL);
    g_object_set(capsfilter, "caps", caps, NULL);
    gst_caps_unref(caps);

    // Ajouter et lier les éléments
    gst_bin_add_many(GST_BIN(pipeline), source, crop, scale, capsfilter, sink, NULL); // , convert
    if (!gst_element_link_many(source, crop, scale, capsfilter, sink, NULL)) { // , convert
        g_printerr("Échec du lien des éléments.\n");
        return -1;
    }


    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    std::string command;

    while (true)
    {
        std::cout << "Enter ZOOM [1-5] or QUIT: ";
        std::vector<std::string> tab = ft_split(ft_getline());
        
		std::cout << "oui" << std::endl;
		int zoom = std::stoi(tab[0]);
		g_object_set(crop,
			"top", 80 * zoom,     // Augmente le crop en haut
			"bottom", 80 * zoom,  // Augmente le crop en bas
			"left", 150 * zoom,   // Augmente le crop à gauche
			"right", 150 * zoom,  // Augmente le crop à droite
			NULL);
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
