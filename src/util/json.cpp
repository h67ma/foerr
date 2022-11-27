#include "json.hpp"
#include <fstream>
#include "../hud/log.hpp"

void writeJsonToFile(const json &root, std::string path)
{
	std::ofstream writer(path);
	writer << std::setfill('\t') << std::setw(1) << root << std::endl;
	writer.close();
}

bool loadJsonFromFile(json &root, std::string path, bool quiet)
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
		root = json::parse(reader, nullptr, true, true);
	}
	catch (const json::parse_error &ex)
	{
		Log::e(STR_ERROR_PARSING_JSON_FILE, path.c_str(), ex.what());
		reader.close(); // no finally :(
		return false;
	}

	// check api version
	int apiVersion = -1;
	parseJsonKey<int>(root, path.c_str(), FOERR_JSON_API_VERSION, apiVersion, true);
	if (apiVersion != JSON_API_VERSION)
		Log::w(STR_JSON_API_VERSION_MISMATCH, path.c_str(), apiVersion, JSON_API_VERSION);

	reader.close();
	Log::v(STR_LOADED_FILE, path.c_str());
	return true;
}

/**
 * Parses a two-element vector from json. Useful for sizes, coordinates, etc.
 * Coordinate element in json looks like this: "key": [123, 456]
 */
bool parseJsonVector2uKey(const json &node, const char* filePath, const char* key, sf::Vector2u &value, bool quiet)
{
	auto search = node.find(key);
	if (search == node.end())
	{
		if (!quiet)
			Log::w(STR_MISSING_KEY, filePath, key);

		return false;
	}

	if (!search->is_array())
	{
		Log::e(STR_INVALID_TYPE, filePath, key);
		return false;
	}

	if (search->size() != 2)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath, key);
		return false;
	}

	try
	{
		value = { (*search)[0], (*search)[1] };
	}
	catch (const json::type_error &ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath, key, ex.what());
		return false;
	}

	return true;
}

/**
 * Parses a three-element vector from json. Useful for sizes, coordinates, etc.
 * Coordinate element in json looks like this: "key": [123, 456, 1]
 */
bool parseJsonVector3uKey(const json &node, const char* filePath, const char* key, Vector3u &value, bool quiet)
{
	auto search = node.find(key);
	if (search == node.end())
	{
		if (!quiet)
			Log::w(STR_MISSING_KEY, filePath, key);

		return false;
	}

	if (!search->is_array())
	{
		Log::e(STR_INVALID_TYPE, filePath, key);
		return false;
	}

	if (search->size() != 3)
	{
		Log::e(STR_INVALID_ARR_SIZE, filePath, key);
		return false;
	}

	try
	{
		value = { (*search)[0], (*search)[1], (*search)[2] };
	}
	catch (const json::type_error &ex)
	{
		Log::e(STR_INVALID_TYPE_EX, filePath, key, ex.what());
		return false;
	}

	return true;
}
