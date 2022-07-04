#include <sstream>
#include "setting.hpp"
#include "../hud/log.hpp"

void Setting::setup(std::string key, uint defaultValue)
{
	this->key = key;
	this->defaultValue.numeric = defaultValue;
	this->val.numeric = defaultValue;
	this->settingType = SETTING_UINT;
}

void Setting::setup(std::string key, bool defaultValue)
{
	this->key = key;
	this->defaultValue.logic = defaultValue;
	this->val.logic = defaultValue;
	this->settingType = SETTING_BOOL;
}

void Setting::setup(std::string key, ScreenCorner defaultValue)
{
	this->key = key;
	this->defaultValue.enumScreenCorner = defaultValue;
	this->val.enumScreenCorner = defaultValue;
	this->settingType = SETTING_ENUM_SCREEN_CORNER;
}

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

void Setting::writeToFile(std::ofstream& file)
{
	switch (this->settingType)
	{
		case SETTING_BOOL:
			file << this->key << '=' << (val.logic ? "true" : "false") << std::endl;
			break;
		case SETTING_ENUM_SCREEN_CORNER:
			file << this->key << '=' << val.enumScreenCorner << std::endl;
			break;
		case SETTING_UINT:
		default:
			file << this->key << '=' << val.numeric << std::endl;
	}
}

/**
 * Tries to load the value from a given line. The line needs to be in correct format.
 * The value will be loaded only if SettingsKey (read from the line) matches the setting's key.
 *
 * @param line a single line read from settings file, in the format "SettingKey=SettingValue" (without quotes).
 * @returns `true` if loading was successful, `false` otherwise.
 */
bool Setting::tryLoadFromLine(std::string line)
{
	// TODO maybe just use xml/json parser
	std::stringstream stream(line);
	std::string key, strVal;
	uint uintVal;

	if (!std::getline(stream, key, '='))
		return false;

	if (key != this->key)
		return false;

	if (!std::getline(stream, strVal))
		return false;

	switch (this->settingType)
	{
		case SETTING_BOOL:
			this->val.logic = strVal == "true" || strVal == "1" || strVal == "yes";
			Log::logStderr(LOG_DEBUG, "Loaded setting %s = %d", key.c_str(), this->val.logic);
			return true;
			break;
		case SETTING_ENUM_SCREEN_CORNER:
			try
			{
				uintVal = stoi(strVal);
			}
			catch (const std::invalid_argument& ex)
			{
				Log::logStderr(LOG_WARNING, "Can't load %s: invalid value", key.c_str());
				return false;
			}
			this->val.enumScreenCorner = static_cast<ScreenCorner>(uintVal);
			Log::logStderr(LOG_DEBUG, "Loaded setting %s = %u", key.c_str(), this->val.enumScreenCorner);
			return true;
			break;
		case SETTING_UINT:
			try
			{
				uintVal = stoi(strVal);
			}
			catch (const std::invalid_argument& ex)
			{
				Log::logStderr(LOG_WARNING, "Can't load %s: invalid value", key.c_str());
				return false;
			}
			this->val.numeric = uintVal;
			Log::logStderr(LOG_DEBUG, "Loaded setting %s = %u", key.c_str(), this->val.numeric);
			return true;
			break;
		default:
			Log::logStderr(LOG_WARNING, "Can't load %s: invalid setting type", key.c_str());
			return false;
	}
}
