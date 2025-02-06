#ifndef GSTREAMERAPP_HPP
# define GSTREAMERAPP_HPP

#include <gst/gst.h>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <vector>

class GstreamerApp
{
	public:
		GstreamerApp();
		GstreamerApp(GstreamerApp const& src);
		GstreamerApp& operator=(GstreamerApp const& src);
		~GstreamerApp();

		void handleUser(void);
	// private:
		void setPipelineState(GstState state);
		void setZoom(float zoom);

		GstElement	*pipeline;
		GstElement	*source;
		GstElement	*decoder;
		GstElement	*cropFilter;
		GstElement	*filter;
		GstElement 	*sink;
		GstBus		bus;
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
std::string ft_getline(void);
std::vector<std::string> ft_split(std::string input);

#endif
