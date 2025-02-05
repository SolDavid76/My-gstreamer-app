#include "GstreamerApp.hpp"

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

int main(int ac, char **av)
{
	try
	{
		gst_init(&ac, &av);
		GstreamerApp app;

		app.setPipelineState(GST_STATE_PLAYING);
		std::cout << "Pipeline is running..." << std::endl;

		app.handleUser();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
