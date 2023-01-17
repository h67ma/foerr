#pragma once

#include <string>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <nlohmann/json.hpp>

#include "../hud/log.hpp"
#include "../consts.hpp"
#include "i18n.hpp"

#define JSON_API_VERSION 1

#define FOERR_JSON_KEY_ID "id"
#define FOERR_JSON_KEY_OFFSET "offset"
#define FOERR_JSON_KEY_VARIANT "var"
#define FOERR_JSON_KEY_BLEND "blend"
#define FOERR_JSON_KEY_MAIN_CNT "main_cnt"
#define FOERR_JSON_KEY_LIGHT_CNT "light_cnt"
#define FOERR_JSON_KEY_CELLS "cells"
#define FOERR_JSON_KEY_BACK_OBJS "back_objs"
#define FOERR_JSON_KEY_FAR_BACK_OBJS "far_back_objs"
#define FOERR_JSON_KEY_BACK_HOLES "back_holes"
#define FOERR_JSON_KEY_BACKWALL "backwall"
#define FOERR_JSON_KEY_LIQUID_LEVEL "liquid_level"
#define FOERR_JSON_KEY_LIQUID_SYMBOL "liquid_symbol"
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
#define FOERR_JSON_KEY_COLOR "color"
#define FOERR_JSON_KEY_MASK "mask"
#define FOERR_JSON_KEY_TYPE "type"
#define FOERR_JSON_KEY_IS_RIGHT "is_right"
#define FOERR_JSON_KEY_OFFSET_LEFT "offset_left"
#define FOERR_JSON_KEY_SPAWN_COORDS "spawn_coords"

using json = nlohmann::json;

void writeJsonToFile(const json &root, const std::string &path);
bool loadJsonFromFile(json &root, const std::string &path, bool quiet = false);
bool parseJsonVector3iKey(const json &node, const std::string &filePath, const char* key, sf::Vector3i &value,
						  bool quiet = false);

/**
 * Parses a two-element vector from json. Useful for sizes, coordinates, etc.
 * Coordinate element in json looks like this: "key": [123, 456]
 */
template<typename T>
bool parseJsonVector2Key(const json &node, const std::string &filePath, const char* key, sf::Vector2<T> &value,
						  bool quiet = false)
{
	auto search = node.find(key);
	if (search == node.end())
	{
		if (!quiet)
			Log::w(STR_MISSING_KEY, filePath.c_str(), key);

		return false;
	}

	if (!search->is_array())
	{
		Log::e(STR_INVALID_TYPE, filePath.c_str(), key);
		return false;
	}

	if (search->size() != 2)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath.c_str(), key, 2, search->size());
		return false;
	}

	try
	{
		value = { (*search)[0], (*search)[1] };
	}
	catch (const json::type_error &ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), key, ex.what());
		return false;
	}

	return true;
}

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
