#include <sstream>
#include "setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

void Setting::setup(std::string key, uint defaultValue, bool debug)
{
	this->key = key;
	this->defaultValue.numeric = defaultValue;
	this->val.numeric = defaultValue;
	this->settingType = SETTING_UINT;
	this->debug = debug;
}

void Setting::setup(std::string key, bool defaultValue, bool debug)
{
	this->key = key;
	this->defaultValue.logic = defaultValue;
	this->val.logic = defaultValue;
	this->settingType = SETTING_BOOL;
	this->debug = debug;
}

void Setting::setup(std::string key, ScreenCorner defaultValue, bool debug)
{
	this->key = key;
	this->defaultValue.enumScreenCorner = defaultValue;
	this->val.enumScreenCorner = defaultValue;
	this->settingType = SETTING_ENUM_SCREEN_CORNER;
	this->debug = debug;
}

// TODO do we actually need this?
void Setting::resetToDefault()
{
	switch (this->settingType)
	{
		case SETTING_BOOL:
			this->val.logic = this->defaultValue.logic;
			break;
		case SETTING_ENUM_SCREEN_CORNER:
			this->val.enumScreenCorner = this->defaultValue.enumScreenCorner;
			break;
		case SETTING_UINT:
		default:
			this->val.numeric = this->defaultValue.numeric;
	}
}

std::string Setting::getKey()
{
	return this->key;
}

bool Setting::isDebug()
{
	return this->debug;
}

/**
 * @returns setting's value, as Json node
 */
Json::Value Setting::getJsonValue()
{
	switch (this->settingType)
	{
		case SETTING_BOOL:
			return Json::Value(val.logic);
			break;
		case SETTING_ENUM_SCREEN_CORNER: // enums are ints anyway
		case SETTING_UINT:
		default:
			return Json::Value(val.numeric);
	}
}

/**
 * Loads value from Json node. Assumes the node's type is the same as setting's type.
 *
 * @param value Json node with an elementary value
 */
void Setting::loadFromJson(Json::Value value)
{
	int readEnum;

	switch (this->settingType)
	{
		case SETTING_BOOL:
			val.logic = value.asBool();
			Log::logStderr(LOG_DEBUG, STR_LOADED_SETTING_D, key.c_str(), val.logic);
			break;
		case SETTING_ENUM_SCREEN_CORNER:
			readEnum = value.asInt();
			if (readEnum >= _CORNER_CNT)
			{
				Log::logStderr(LOG_WARNING, STR_INVALID_VALUE, readEnum, key.c_str());
				return;
			}
			val.enumScreenCorner = (ScreenCorner)readEnum;
			Log::logStderr(LOG_DEBUG, STR_LOADED_SETTING_D, key.c_str(), val.enumScreenCorner);
			break;
		case SETTING_UINT:
		default:
			val.numeric = value.asUInt();
			Log::logStderr(LOG_DEBUG, STR_LOADED_SETTING_U, key.c_str(), val.numeric);
	}
}
