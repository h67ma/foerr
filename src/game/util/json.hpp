#pragma once

#include <nlohmann/json.hpp>
#include <SFML/System/Vector2.hpp>
#include "i18n.hpp"

using json = nlohmann::json;

void writeJsonToFile(const json &root, std::string path);
bool loadJsonFromFile(json &root, std::string path);
bool parseJsonVector2uKey(const json &node, const char* filePath, const char* key, sf::Vector2u &value, bool quiet=false);

template<typename T>
bool parseJsonKey(const json &node, const char* filePath, const char* key, T &value, bool quiet=false)
{
	auto search = node.find(key);
	if (search == node.end())
	{
		if (!quiet)
			Log::w(STR_MISSING_KEY, filePath, key);

		return false;
	}

	try
	{
		value = search.value();
	}
	catch (const json::type_error &ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath, key, ex.what());
		return false;
	}

	return true;
}
