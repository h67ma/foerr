#pragma once

#include <string>
#include <SFML/System/Vector2.hpp>
#include <json/reader.h>
#include "../consts.hpp"
#include "../hud/hud.hpp"

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
	return path1 + '/' + path2;
}

bool loadJsonFromFile(Json::Value &root, std::string path);

bool parseJsonStringKey(Json::Value &node, const char* filePath, const char* key, std::string &value, bool quiet=false);
bool parseJsonBoolKey(Json::Value &node, const char* filePath, const char* key, bool &value, bool quiet=false);
bool parseJsonUintKey(Json::Value &node, const char* filePath, const char* key, uint &value, bool quiet=false);
bool parseJsonVector2uKey(Json::Value &node, const char* filePath, const char* key, sf::Vector2u &value, bool quiet=false);
uint getFontSize(GuiScale scale, FontSize size);
uint getFontGap(GuiScale scale, FontSize size);
