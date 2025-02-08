#include "GstreamerApp.hpp"

bool checkFile(const std::string& path)
{
	std::ifstream file(path);
	if (file.good())
	{
		file.close();
		return (true);
	}
	return (false);
}

std::string ft_getline(void)
{
	std::string res;

	while (!std::cin.eof() && res.empty())
	{
		std::cout << "$>";
		std::getline(std::cin, res);
	}

	if (std::cin.eof())
		throw ft_exception("ctrl+D");
	return (res);
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

int ft_stoi(const std::string& str)
{
	int res = 0;
	int sign = 1;
	size_t i = 0;

	if (0 < str.size() && (str[0] == '-' || str[0] == '+'))
	{
		if (str[0] == '-')
			sign = -1;
		i++;
	}

	for (; i < str.size(); i++)
	{
		if (str[i] < '0' || '9' < str[i])
			return (0);
		res = res * 10 + (str[i] - '0');
	}

	return (res * sign);
}
