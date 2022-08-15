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

	if (!parseJsonBoolKey(root, indexPath.c_str(), FOERR_JSON_KEY_TYPE_GRIND, this->isGrind))
		return false;

	if (!parseJsonBoolKey(root, indexPath.c_str(), FOERR_JSON_KEY_TYPE_BASECAMP, this->isBasecamp))
		return false;

	if (this->isBasecamp && this->isGrind)
	{
		// something stinks here...
		Log::e(STR_LOC_INVALID_TYPES, indexPath.c_str());
		return false;
	}

	if (!parseJsonStringKey(root, indexPath.c_str(), FOERR_JSON_KEY_BACKGROUND_FULL, backgroundFullPath))
		return false;

	this->backgroundFull = resMgr.loadImage(backgroundFullPath);
	if (this->backgroundFull == nullptr)
		return false;

	if (!parseJsonUintKey(root, indexPath.c_str(), FOERR_JSON_KEY_WIDTH, this->width))
		return false;

	if (!parseJsonUintKey(root, indexPath.c_str(), FOERR_JSON_KEY_HEIGHT, this->height))
		return false;

	if (!root.isMember(FOERR_JSON_KEY_ROOM_MAP))
	{
		Log::e(STR_MISSING_KEY, indexPath.c_str(), FOERR_JSON_KEY_ROOM_MAP);
		return false;
	}

	// TODO load room map, verify width/height are correct

	Log::d(STR_LOADED_LOCATION, indexPath.c_str());

	return true;
}
