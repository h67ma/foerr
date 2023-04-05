#include "util.hpp"

#include <stdexcept>
#include <sstream>

void splitString(std::vector<std::string> &tokens, const std::string &input, char delim)
{
	std::stringstream ss(input);
	std::string tmpToken;

	while (getline(ss, tmpToken, delim))
		tokens.push_back(tmpToken);
}

bool strToInt(const std::string &input, int &output)
{
	try
	{
		output = std::stoi(input);
	}
	catch (std::invalid_argument const& ex)
	{
		return false;
	}
	catch (std::out_of_range const& ex)
	{
		return false;
	}

	return true;
}
