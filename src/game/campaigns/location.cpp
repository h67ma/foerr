#include <json/reader.h>
#include "location.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

/**
 * Loads all rooms located in `locDir` (json files, each one contains a single room).
 * Also loads the "big" background image for this location.
 *
 * Location file structure:
 * {
 *	"grind": false,
 *	"basecamp": false,
 *	"background_full": "path/to/img.png",
 *	"width": 2,
 *	"height": 3,
 *	"room_map": [
 *		["one",		"two"],
 *		["three",	"/EMPTY"],
 *		["five",	"six"]
 *	]
* }
 */
bool Location::load(std::string locDir, ResourceManager& resMgr)
{
	std::string indexPath = pathCombine(locDir, std::string(FILENAME_INDEX));
	std::string backgroundFullPath;
	Json::Value root;

	if (!loadJsonFromFile(root, indexPath))
		return false;

	// check if all keys are present

	if (!root.isMember(FOERR_JSON_KEY_TYPE_GRIND))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_TYPE_GRIND);
		return false;
	}

	if (!root.isMember(FOERR_JSON_KEY_TYPE_BASECAMP))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_TYPE_BASECAMP);
		return false;
	}

	if (!root.isMember(FOERR_JSON_KEY_BACKGROUND_FULL))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_BACKGROUND_FULL);
		return false;
	}

	if (!root.isMember(FOERR_JSON_KEY_WIDTH))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_WIDTH);
		return false;
	}

	if (!root.isMember(FOERR_JSON_KEY_HEIGHT))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_HEIGHT);
		return false;
	}

	if (!root.isMember(FOERR_JSON_KEY_ROOM_MAP))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		return false;
	}

	// parse values

	try
	{
		this->isGrind = root[FOERR_JSON_KEY_TYPE_GRIND].asBool();
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, indexPath.c_str(), ex.what());
		return false;
	}

	try
	{
		this->isBasecamp = root[FOERR_JSON_KEY_TYPE_BASECAMP].asBool();
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, indexPath.c_str(), ex.what());
		return false;
	}

	if (this->isBasecamp && this->isGrind)
	{
		// something stinks here...
		Log::e(STR_LOC_INVALID_TYPES, indexPath.c_str());
		return false;
	}

	try
	{
		backgroundFullPath = std::string(root[FOERR_JSON_KEY_BACKGROUND_FULL].asCString());
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, indexPath.c_str(), ex.what());
		return false;
	}

	try
	{
		this->width = root[FOERR_JSON_KEY_WIDTH].asUInt();
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, indexPath.c_str(), ex.what());
		return false;
	}

	try
	{
		this->width = root[FOERR_JSON_KEY_HEIGHT].asUInt();
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, indexPath.c_str(), ex.what());
		return false;
	}

	this->backgroundFull = resMgr.loadImage(backgroundFullPath);
	if (this->backgroundFull == nullptr)
		return false;

	// TODO load room map

	Log::d(STR_LOADED_LOCATION, indexPath.c_str());

	return true;
}
