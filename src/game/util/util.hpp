#pragma once

#include <json/reader.h>
#include <string>

// stolen from https://stackoverflow.com/questions/63121776/simplest-syntax-for-string-interpolation-in-c
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
	return path1 + '/' + path2;
}

bool loadJsonFromFile(Json::Value &root, std::string path);

bool parseJsonStringKey(Json::Value &node, const char* filePath, const char* key, std::string &value);
