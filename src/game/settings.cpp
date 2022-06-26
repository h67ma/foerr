#include <fstream>
#include "settings.hpp"
#include "consts.h"

#define INIT_SETTING(sKey, sDefault) this->_settings[sKey].setup(#sKey, sDefault)

Settings::Settings()
{
	// window
	INIT_SETTING(SETT_FULLSCREEN_ENABLED, false);
	INIT_SETTING(SETT_FPS_LIMIT_ENABLED, true);
	INIT_SETTING(SETT_FPS_LIMIT, 60U);
	INIT_SETTING(SETT_FAKE_VSYNC_ENABLED, false);
	INIT_SETTING(SETT_WINDOW_WIDTH, 1280U);
	INIT_SETTING(SETT_WINDOW_HEIGHT, 720U);
	// TODO? windowed size + position override

	// logging
	INIT_SETTING(SETT_DISPLAY_DEBUG_MSGS_IN_LOG, true);
	INIT_SETTING(SETT_WRITE_DEBUG_LOG_TO_FILE, true);

	// font sizes
	INIT_SETTING(SETT_NORMAL_FONT_SIZE, 18U);
	INIT_SETTING(SETT_NORMAL_FONT_SIZE_WITH_GAP, 20U);
	INIT_SETTING(SETT_SMALL_FONT_SIZE, 14U);

	// gui
	INIT_SETTING(SETT_PREFER_CUSTOM_CURSOR, true);
	INIT_SETTING(SETT_SHOW_FPS_COUNTER, true);
	INIT_SETTING(SETT_ANCHOR_LOG, CORNER_TOP_RIGHT);
	//INIT_SETTING(SETT_LOG_MSG_TIMEOUT, 3); // is this really necessary?
}

void Settings::saveConfig()
{
	std::ofstream file(PATH_SETTINGS);

	for (size_t i = 0; i < _SETTINGS_CNT; i++)
	{
		this->_settings[i].writeToFile(file);
	}

	file.close();
}

void Settings::loadConfig()
{
	std::string line;
	std::ifstream file(PATH_SETTINGS);

	if (!file.is_open())
		return; // we'll just run on default settings (which are already assigned)

	while (getline(file, line))
	{
		for (size_t i = 0; i < _SETTINGS_CNT; i++)
		{
			if (this->_settings[i].tryLoadFromLine(line))
				continue; // setting found & load successful, we can move to the next setting

			// if we've scanned all lines and the setting was still not found, it will just use the default value
		}
	}

	file.close();
}

uint Settings::getUint(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return 0; // default value is better than crash

	return this->_settings[idx].val.numeric;
}

bool Settings::getBool(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return false; // default value is better than crash

	return this->_settings[idx].val.logic;
}

ScreenCorner Settings::getScreenCorner(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return CORNER_TOP_LEFT; // default value is better than crash

	return this->_settings[idx].val.enumScreenCorner;
}

void Settings::setUint(SettingName idx, uint newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->_settings[idx].val.numeric = newValue;
}

void Settings::setBool(SettingName idx, bool newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->_settings[idx].val.logic = newValue;
}

void Settings::setScreenCorner(SettingName idx, ScreenCorner newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->_settings[idx].val.enumScreenCorner = newValue;
}
