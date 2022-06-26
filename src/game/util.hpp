#pragma once

#include <string>
#include <vector>
#include <iostream>

// stolen from https://stackoverflow.com/questions/63121776/simplest-syntax-for-string-interpolation-in-c
template<typename... T>
std::string litSprintf(const char *fmt, T... args)
{
	const size_t actualSize = snprintf(nullptr, 0, fmt, args...) + 1;
	char buf[actualSize];
	snprintf(buf, actualSize, fmt, args...);
	return std::string(buf);
}
