#include "color.hpp"
#include "../consts.hpp"

/**
 * Parses a color string.
 * Valid formats are:
 *   - /#[0-9a-f]{3}/i
 *   - /#[0-9a-f]{6}/i
 */
bool Color::loadFromColorString(std::string input)
{
	uint r, g, b;

	if (input.size() == 7)
	{
		if (sscanf(input.c_str(), "#%02x%02x%02x", &r, &g, &b) < 3)
			return false;

		this->r = r;
		this->g = g;
		this->b = b;
		return true;
	}
	else if (input.size() == 4)
	{
		if (sscanf(input.c_str(), "#%1x%1x%1x", &r, &g, &b) < 3)
			return false;

		this->r = r * 16;
		this->g = g * 16;
		this->b = b * 16;
		return true;
	}

	return false;
}

std::string Color::toString()
{
	char buf[8];
	sprintf(buf, "#%02x%02x%02x", this->r, this->g, this->b);
	return std::string(buf);
}
