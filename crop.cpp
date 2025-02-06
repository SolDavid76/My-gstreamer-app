#include <gst/gst.h>
#include <iostream>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Création du pipeline
    GstElement *pipeline = gst_pipeline_new("video-zoom-pipeline");
    GstElement *source = gst_element_factory_make("v4l2src", "source");
    GstElement *convert = gst_element_factory_make("videoconvert", "convert");
    GstElement *crop = gst_element_factory_make("videocrop", "crop");
    GstElement *scale = gst_element_factory_make("videoscale", "scale");
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !convert || !crop || !scale || !capsfilter || !sink) {
        g_printerr("Échec de la création des éléments.\n");
        return -1;
    }

    // 📌 Configuration initiale du crop
    g_object_set(crop,
        "top", 50,
        "bottom", 50,
        "left", 100,
        "right", 100,
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
    gst_bin_add_many(GST_BIN(pipeline), source, convert, crop, scale, capsfilter, sink, NULL);
    if (!gst_element_link_many(source, convert, crop, scale, capsfilter, sink, NULL)) {
        g_printerr("Échec du lien des éléments.\n");
        return -1;
    }

    // Démarrer le pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    g_print("Appuyez sur Entrée pour modifier le zoom...\n");
    getchar();  // Attendre une entrée utilisateur

    // 📌 Modifier dynamiquement le zoom (crop) après l'entrée utilisateur
    g_print("Modification du zoom...\n");
    g_object_set(crop,
        "top", 80,     // Augmente le crop en haut
        "bottom", 80,  // Augmente le crop en bas
        "left", 150,   // Augmente le crop à gauche
        "right", 150,  // Augmente le crop à droite
        NULL);

    g_print("Appuyez sur Entrée pour modifier le zoom...\n");
    getchar();  // Attendre une entrée utilisateur
	
	g_print("Modification du zoom...\n");
    g_object_set(crop,
        "top", 0,     // Augmente le crop en haut
        "bottom", 0,  // Augmente le crop en bas
        "left", 0,   // Augmente le crop à gauche
        "right", 0,  // Augmente le crop à droite
        NULL);

    g_print("Zoom modifié ! Appuyez sur Entrée pour quitter...\n");
    getchar();  // Attendre encore une entrée avant de quitter

    // Nettoyage
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    
    return 0;
}
