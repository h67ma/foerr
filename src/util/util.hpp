// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <vector>

#include <SFML/System/Vector2.hpp>

#include "../consts.hpp"

#define STR(thing) #thing
#define STR_EXP(thing) STR(thing) // "the double expansion trick"

constexpr char PATH_DELIM = '/';

// stolen from https://stackoverflow.com/questions/63121776/simplest-syntax-for-string-interpolation-in-c
// TODO should be a part of Translator
template<typename... T>
std::string litSprintf(const char* fmt, T... args)
{
	// prevent directly printing std::string and such
	// clang-format off
	static_assert(((std::is_same_v<T, const char*> ||
					std::is_same_v<T, int> ||
					std::is_same_v<T, uint> ||
					std::is_same_v<T, float> ||
					std::is_same_v<T, std::size_t> ||
					std::is_same_v<T, char> ||
					std::is_enum_v<T>) && ...), "Illegal argument type for snprintf");
	// clang-format on

	const size_t actualSize = snprintf(nullptr, 0, fmt, args...) + 1;
	char* buf = new char[actualSize];
	snprintf(buf, actualSize, fmt, args...);
	std::string ret = std::string(buf);
	delete[] buf;
	return ret;
}

inline std::string pathCombine(const std::string& path1, const std::string& path2)
{
	return path1 + PATH_DELIM + path2;
}

/**
 * eh, why not
 */
inline std::string pathCombine(const std::string& path1, const std::string& path2, const std::string& path3)
{
	return path1 + PATH_DELIM + path2 + PATH_DELIM + path3;
}

#define COLOR_GRAY(shade) sf::Color(shade, shade, shade)
#define COLOR_GRAY_ALPHA(shade, alpha) sf::Color(shade, shade, shade, alpha)
#define COLOR_ALPHA(alpha) sf::Color(0xFF, 0xFF, 0xFF, alpha)
// clang-format off
#define DIM_COLOR(color, shade) ((color) * COLOR_GRAY(shade))
// clang-format on

/**
 * "Just do it like the Boost guys did it" ~ SO proverb, circa 2010
 */
template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	seed ^= std::hash<T> {}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

void splitString(std::vector<std::string>& tokens, const std::string& input, char delim);
bool strToInt(const std::string& input, int& output);

void operator-=(sf::Vector2i& lhs, sf::Vector2f rhs);
sf::Vector2u operator+(sf::Vector2f lhs, sf::Vector2u rhs);

/**
 * Divides and ceils an uint value.
 */
constexpr uint uintDivCeil(uint dividend, uint divisor)
{
	return (dividend / divisor) + static_cast<uint>((dividend % divisor) != 0);
}
