#pragma once

#include <string>
#include "../consts.hpp"
#include "../hud/hud.hpp"

#define PATH_DELIM '/'

// stolen from https://stackoverflow.com/questions/63121776/simplest-syntax-for-string-interpolation-in-c
// TODO should be a part of Translator
template<typename... T>
std::string litSprintf(const char *fmt, T... args)
{
	const size_t actualSize = snprintf(nullptr, 0, fmt, args...) + 1;
	char *buf = new char[actualSize];
	snprintf(buf, actualSize, fmt, args...);
	std::string ret = std::string(buf);
	delete[] buf;
	return ret;
}

inline std::string pathCombine(std::string path1, std::string path2)
{
	return path1 + PATH_DELIM + path2;
}

/**
 * eh, why not
 */
inline std::string pathCombine(std::string path1, std::string path2, std::string path3)
{
	return path1 + PATH_DELIM + path2 + PATH_DELIM + path3;
}

uint getFontSize(GuiScale scale, FontSize size);
uint getFontGap(GuiScale scale, FontSize size);

#define DIM_COLOR(color, shade) color * sf::Color(shade, shade, shade)
