#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include "../hud/log.hpp"
#include "i18n.hpp"
#include "../consts.hpp"

#define JSON_API_VERSION 1

#define FOERR_JSON_KEY_CELLS "cells"
#define FOERR_JSON_KEY_BACKWALL "backwall"
#define FOERR_JSON_KEY_BACKGROUND_FULL "background_full"
#define FOERR_JSON_KEY_WORLDMAP_ICON "worldmap_icon"
#define FOERR_JSON_KEY_WORLDMAP_COORDS "worldmap_coords"
#define FOERR_JSON_KEY_COORDS "coords"
#define FOERR_JSON_KEY_IS_START "is_start"
#define FOERR_JSON_KEY_WORLDMAP_ICON_BIG "worldmap_icon_big"
#define FOERR_JSON_KEY_WORLDMAP_BACKGROUND "worldmap_background"
#define FOERR_JSON_KEY_TYPE_GRIND "grind"
#define FOERR_JSON_KEY_TYPE_BASECAMP "basecamp"
#define FOERR_JSON_KEY_RECOMMENDED_LVL "rec_lvl"
#define FOERR_JSON_KEY_ROOMS "rooms"
#define FOERR_JSON_KEY_LOCATIONS "locations"
#define FOERR_JSON_KEY_KEYS "keys"
#define FOERR_JSON_KEY_SETTINGS "settings"
#define FOERR_JSON_KEY_TITLE "title"
#define FOERR_JSON_KEY_DESCRIPTION "description"
#define FOERR_JSON_KEY_START_LOC "start_location"
#define FOERR_JSON_API_VERSION "api_version"
#define FOERR_JSON_KEY_SOLIDS "solids"
#define FOERR_JSON_KEY_OTHER "other"
#define FOERR_JSON_KEY_TEXTURE "txt"
#define FOERR_JSON_KEY_TEXTURE_DELIM "txt_delim"
#define FOERR_JSON_KEY_TEXTURE_DELIM_OFFSET "txt_delim_offset"
#define FOERR_JSON_KEY_MASK "mask"
#define FOERR_JSON_KEY_TYPE "type"
#define FOERR_JSON_KEY_IS_RIGHT "is_right"
#define FOERR_JSON_KEY_OFFSET_LEFT "offset_left"

using json = nlohmann::json;

void writeJsonToFile(const json &root, std::string path);
bool loadJsonFromFile(json &root, std::string path, bool quiet = false);
bool parseJsonVector2uKey(const json &node, const std::string &filePath, const char* key, sf::Vector2u &value,
						  bool quiet = false); // TODO template
bool parseJsonVector2iKey(const json &node, const std::string &filePath, const char* key, sf::Vector2i &value,
						  bool quiet = false); // TODO template
bool parseJsonVector3iKey(const json &node, const std::string &filePath, const char* key, sf::Vector3i &value,
						  bool quiet = false);

template<typename T>
bool parseJsonKey(const json &node, const std::string &filePath, const char* key, T &value, bool quiet = false)
{
	auto search = node.find(key);
	if (search == node.end())
	{
		if (!quiet)
			Log::w(STR_MISSING_KEY, filePath.c_str(), key);

		return false;
	}

	try
	{
		value = search.value();
	}
	catch (const json::type_error &ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), key, ex.what());
		return false;
	}

	return true;
}
