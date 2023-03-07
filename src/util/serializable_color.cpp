#include "serializable_color.hpp"

#include "../consts.hpp"

SerializableColor& SerializableColor::operator=(const sf::Color &color)
{
	this->r = color.r;
	this->g = color.g;
	this->b = color.b;
	this->a = color.a;
	return *this;
}

/**
 * Parses a color string.
 * Valid formats are:
 *   - /#[0-9a-f]{3}/i
 *   - /#[0-9a-f]{6}/i
 */
bool SerializableColor::loadFromColorString(const std::string &input)
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

std::string SerializableColor::toString() const
{
	char buf[8];
	snprintf(buf, sizeof(buf), "#%02x%02x%02x", this->r, this->g, this->b);
	return std::string(buf);
}
