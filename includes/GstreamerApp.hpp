#ifndef GSTREAMERAPP_HPP
# define GSTREAMERAPP_HPP

#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <exception>
#include <vector>

class GstreamerApp
{
	public:
		GstreamerApp(int ac, char **av);
		GstreamerApp(GstreamerApp const& src);
		GstreamerApp& operator=(GstreamerApp const& src);
		~GstreamerApp();

		void printHelp(void);
		void handleUser(void);
		void setPipelineState(GstState state);
	private:
		void setSource(int ac, char **av);
		int  setZoom(int zoomLevel);

		int			width;
		int			height;

		GstElement	*pipeline;
		GstElement	*source;
		GstElement	*decoder;
		GstElement	*cropFilter;
		GstElement	*scaleFilter;
		GstElement	*textoverlay;
		GstElement	*capsfilter;
		GstElement	*sink;
		GstBus		*bus;
};

class ft_exception: public std::exception
{
	public:
		ft_exception(std::string errorMessage) throw(): _errorMessage(errorMessage) {}
		virtual const char* what() const throw() { return (this->_errorMessage.c_str()); }
		virtual ~ft_exception() throw() {}
	private:
		std::string _errorMessage;
};

/* main.cpp */
void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
gboolean handleBus(GstBus *bus, GstMessage *msg, gpointer user_data);

/* utils.cpp */
bool						checkFile(const std::string& path);
std::string					ft_getline(void);
std::vector<std::string>	ft_split(std::string input);
int							ft_stoi(const std::string& str);

#endif
