// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "json.hpp"

#include <fstream>

#include "../hud/log.hpp"

void writeJsonToFile(const nlohmann::json& root, const std::string& path)
{
	std::ofstream writer(path);
	writer << std::setfill('\t') << std::setw(1) << root << std::endl;
	writer.close();
}

/**
 * @brief Loads a json file.
 *
 * Automatically checks if the file contains a proper key with api version, and if the version equals game api version.
 *
 * @param root reference to json root
 * @param path path to json file to load
 * @param quiet if true, warnings won't be outputted
 * @return true if loading succeeded
 * @return false if loading failed
 */
bool loadJsonFromFile(nlohmann::json& root, const std::string& path, bool quiet)
{
	std::ifstream reader(path);

	if (!reader.is_open())
	{
		if (!quiet)
			Log::e(STR_FILE_OPEN_ERROR, path.c_str());
		return false;
	}

	try
	{
		root = nlohmann::json::parse(reader, nullptr, true, true);
	}
	catch (const nlohmann::json::parse_error& ex)
	{
		Log::e(STR_ERROR_PARSING_JSON_FILE, path.c_str(), ex.what());
		reader.close(); // no finally :(
		return false;
	}

	// check api version
	int apiVersion = -1;
	parseJsonKey<int>(root, path, FOERR_JSON_KEY_API_VERSION, apiVersion, true);
	if (apiVersion != JSON_API_VERSION)
		Log::w(STR_JSON_API_VERSION_MISMATCH, path.c_str(), apiVersion, JSON_API_VERSION);

	reader.close();
	Log::v(STR_LOADED_FILE, path.c_str());
	return true;
}

/**
 * Parses a three-element vector from json. Useful for sizes, coordinates, etc.
 * Coordinate element in json looks like this: "key": [123, 456, 1]
 */
bool parseJsonVector3iKey(const nlohmann::json& node, const std::string& filePath, const char* key, sf::Vector3i& value,
						  bool quiet)
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

	if (search->size() != 3)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath.c_str(), key, 3, search->size());
		return false;
	}

	try
	{
		value = { (*search)[0], (*search)[1], (*search)[2] };
	}
	catch (const nlohmann::json::type_error& ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath.c_str(), key, ex.what());
		return false;
	}

	return true;
}
