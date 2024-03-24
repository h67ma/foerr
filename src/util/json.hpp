// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <nlohmann/json.hpp>

#include "../consts.hpp"
#include "../hud/log.hpp"
#include "i18n.hpp"

constexpr int JSON_API_VERSION = 1;

const std::string FOERR_JSON_KEY_ID = "id";
const std::string FOERR_JSON_KEY_OFFSET = "offset";
const std::string FOERR_JSON_KEY_OFFSET_LIGHT = "offset_light";
const std::string FOERR_JSON_KEY_VARIANT = "var";
const std::string FOERR_JSON_KEY_BLEND = "blend";
const std::string FOERR_JSON_KEY_ALPHA = "alpha";
const std::string FOERR_JSON_KEY_MAIN_CNT = "main_cnt";
const std::string FOERR_JSON_KEY_LIGHT_CNT = "light_cnt";
const std::string FOERR_JSON_KEY_CELLS = "cells";
const std::string FOERR_JSON_KEY_BACK_OBJS = "back_objs";
const std::string FOERR_JSON_KEY_FAR_BACK_OBJS = "far_back_objs";
const std::string FOERR_JSON_KEY_BACK_HOLES = "back_holes";
const std::string FOERR_JSON_KEY_LIGHTS_ON = "lights_on";
const std::string FOERR_JSON_KEY_BACKWALL = "backwall";
const std::string FOERR_JSON_KEY_LIQUID_LEVEL = "liquid_level";
const std::string FOERR_JSON_KEY_LIQUID_SYMBOL = "liquid_symbol";
const std::string FOERR_JSON_KEY_BACKGROUND_FULL = "background_full";
const std::string FOERR_JSON_KEY_WORLDMAP_ICON = "worldmap_icon";
const std::string FOERR_JSON_KEY_WORLDMAP_COORDS = "worldmap_coords";
const std::string FOERR_JSON_KEY_COORDS = "coords";
const std::string FOERR_JSON_KEY_IS_START = "is_start";
const std::string FOERR_JSON_KEY_WORLDMAP_ICON_BIG = "worldmap_icon_big";
const std::string FOERR_JSON_KEY_TYPE_GRIND = "grind";
const std::string FOERR_JSON_KEY_TYPE_BASECAMP = "basecamp";
const std::string FOERR_JSON_KEY_RECOMMENDED_LVL = "rec_lvl";
const std::string FOERR_JSON_KEY_ROOMS = "rooms";
const std::string FOERR_JSON_KEY_LOCATIONS = "locations";
const std::string FOERR_JSON_KEY_KEYS = "keys";
const std::string FOERR_JSON_KEY_SETTINGS = "settings";
const std::string FOERR_JSON_KEY_TITLE = "title";
const std::string FOERR_JSON_KEY_DESCRIPTION = "description";
const std::string FOERR_JSON_KEY_START_LOC = "start_location";
const std::string FOERR_JSON_KEY_API_VERSION = "api_version";
const std::string FOERR_JSON_KEY_SOLIDS = "solids";
const std::string FOERR_JSON_KEY_OTHER = "other";
const std::string FOERR_JSON_KEY_TEXTURE = "txt";
const std::string FOERR_JSON_KEY_TEXTURE_DELIM = "txt_delim";
const std::string FOERR_JSON_KEY_TEXTURE_DELIM_OFFSET = "txt_delim_offset";
const std::string FOERR_JSON_KEY_COLOR = "color";
const std::string FOERR_JSON_KEY_MASK = "mask";
const std::string FOERR_JSON_KEY_TYPE = "type";
const std::string FOERR_JSON_KEY_IS_RIGHT = "is_right";
const std::string FOERR_JSON_KEY_OFFSET_LEFT = "offset_left";
const std::string FOERR_JSON_KEY_SPAWN_COORDS = "spawn_coords";

void writeJsonToFile(const nlohmann::json& root, const std::string& path);
bool loadJsonFromFile(nlohmann::json& root, const std::string& path, bool quiet = false);
bool parseJsonVector3iKey(const nlohmann::json& node, const std::string& filePath, const std::string& key,
						  sf::Vector3i& value, bool quiet = false);

/**
 * Parses a two-element vector from json. Useful for sizes, coordinates, etc.
 * Coordinate element in json looks like this: "key": [123, 456]
 */
template<typename T>
bool parseJsonVector2Key(const nlohmann::json& node, const std::string& filePath, const std::string& key,
						 sf::Vector2<T>& value, bool quiet = false)
{
	auto search = node.find(key);
	if (search == node.end())
	{
		if (!quiet)
			Log::w(STR_MISSING_KEY, filePath.c_str(), key.c_str());

		return false;
	}

	if (!search->is_array())
	{
		Log::e(STR_INVALID_TYPE, filePath.c_str(), key.c_str());
		return false;
	}

	if (search->size() != 2)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath.c_str(), key.c_str(), 2, search->size());
		return false;
	}

	try
	{
		value = { (*search)[0], (*search)[1] };
	}
	catch (const nlohmann::json::type_error& ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), key.c_str(), ex.what());
		return false;
	}

	return true;
}

template<typename T>
bool parseJsonKey(const nlohmann::json& node, const std::string& filePath, const std::string& key, T& value,
				  bool quiet = false)
{
	auto search = node.find(key);
	if (search == node.end())
	{
		if (!quiet)
			Log::w(STR_MISSING_KEY, filePath.c_str(), key.c_str());

		return false;
	}

	try
	{
		value = search.value();
	}
	catch (const nlohmann::json::type_error& ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), key.c_str(), ex.what());
		return false;
	}

	return true;
}
