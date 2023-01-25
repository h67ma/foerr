#include "util.hpp"

#include <stdexcept>
#include <sstream>

uint getFontSize(GuiScale scale, FontSize size)
{
	if (scale == GUI_SMALL)
	{
		switch (size)
		{
			case FONT_H1:
				return 30;
			case FONT_H2:
				return 18;
			case FONT_H3:
				return 14;
			case FONT_SPAN:
			default:
				return 13;
		}
	}
	else if (scale == GUI_LARGE)
	{
		switch (size)
		{
			case FONT_H1:
				return 52;
			case FONT_H2:
				return 32;
			case FONT_H3:
				return 24;
			case FONT_SPAN:
			default:
				return 22;
		}
	}
	else // GUI_NORMAL
	{
		switch (size)
		{
			case FONT_H1:
				return 40;
			case FONT_H2:
				return 23;
			case FONT_H3:
				return 18;
			case FONT_SPAN:
			default:
				return 16;
		}
	}
}

/**
 * Returns the vertical offset used for centering text.
 * The return value should be added to text Y coordinate.
 */
int getFontVOffset(GuiScale scale, FontSize size)
{
	if (scale == GUI_SMALL)
	{
		switch (size)
		{
			case FONT_H1:
				return -7;
			case FONT_H2:
				return -4;
			case FONT_H3:
			case FONT_SPAN:
			default:
				return -3;
		}
	}
	else if (scale == GUI_LARGE)
	{
		switch (size)
		{
			case FONT_H1:
				return -13;
			case FONT_H2:
				return -7;
			case FONT_H3:
			case FONT_SPAN:
			default:
				return -5;
		}
	}
	else // GUI_NORMAL
	{
		switch (size)
		{
			case FONT_H1:
				return -10;
			case FONT_H2:
				return -5;
			case FONT_H3:
				return -4;
			case FONT_SPAN:
			default:
				return -3;
		}
	}
}

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
