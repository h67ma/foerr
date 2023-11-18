// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

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

void operator-=(sf::Vector2i &lhs, sf::Vector2f rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
}

sf::Vector2u operator+(sf::Vector2f lhs, sf::Vector2u rhs)
{
	rhs.x += lhs.x;
	rhs.y += lhs.y;
	return rhs;
}
