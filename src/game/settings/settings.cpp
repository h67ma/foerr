#include <fstream>
#include "settings.hpp"
#include "../consts.h"
#include "../hud/log.hpp"

Settings::Settings()
{
	// window
	this->settings[SETT_FULLSCREEN_ENABLED].setup("FullscreenEnabled", false);
	this->settings[SETT_FPS_LIMIT_ENABLED].setup("FpsLimitEnabled", true);
	this->settings[SETT_FPS_LIMIT].setup("FpsLimit", 60U);
	this->settings[SETT_FAKE_VSYNC_ENABLED].setup("FakeVsyncEnabled", false);
	// TODO? windowed size + position override

	// logging
	this->settings[SETT_WRITE_LOG_TO_FILE].setup("WriteLogToFile", true);

	// hud
	this->settings[SETT_PREFER_CUSTOM_CURSOR].setup("PreferCustomCursor", true);
	this->settings[SETT_SHOW_FPS_COUNTER].setup("ShowFpsCounter", true);
	this->settings[SETT_ANCHOR_LOG].setup("LogAnchor", CORNER_TOP_RIGHT);
	this->settings[SETT_ANCHOR_FPS].setup("FpsAnchor", CORNER_TOP_LEFT);
	//this->settings[SETT_LOG_MSG_TIMEOUT].setup("LogMsgTimeout", 3); // is this really necessary?
	// TODO this->settings[SETT_FONT_SCALING_FACTOR].setup("FontScalingFactor", 1.0);

	// debug
	this->settings[SETT_WINDOW_WIDTH].setup("WindowW", 1280U, true);
	this->settings[SETT_WINDOW_HEIGHT].setup("WindowH", 720U, true);
	this->settings[SETT_DISPLAY_DEBUG_MSGS_IN_LOG].setup("DisplayDebugMsgsInLog", false, true);
	// TODO this->settings[SETT_SHOW_BOUNDING_BOXEN].setup("ShowBoundingBoxen", false, true); // Brian, what's the plural form of "box"?
	// TODO this->settings[SETT_DEV_CONSOLE_ENABLED].setup("DevConsoleEnabled", false, true);
}

void Settings::saveConfig()
{
	std::ofstream file(PATH_SETTINGS);

	for (size_t i = 0; i < _SETTINGS_CNT; i++)
	{
		this->settings[i].writeToFile(file);
	}

	file.close();

	Log::logStderr(LOG_DEBUG, "Saved settings");
}

void Settings::loadConfig()
{
	std::string line;
	std::ifstream file(PATH_SETTINGS);

	if (!file.is_open())
		return; // we'll just run on default settings (which are already assigned)

	while (getline(file, line))
	{
		bool found = false;
		for (size_t i = 0; i < _SETTINGS_CNT; i++)
		{
			if (this->settings[i].tryLoadFromLine(line))
			{
				found = true;
				continue; // setting found & load successful, we can move to the next setting
			}
		}

		if (!found)
			Log::logStderr(LOG_WARNING, "Failed to parse settings line:\n%s", line.c_str());
	}

	file.close();
}

uint Settings::getUint(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return 0; // default value is better than crash

	return this->settings[idx].val.numeric;
}

bool Settings::getBool(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return false; // default value is better than crash

	return this->settings[idx].val.logic;
}

ScreenCorner Settings::getScreenCorner(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return CORNER_TOP_LEFT; // default value is better than crash

	return this->settings[idx].val.enumScreenCorner;
}

void Settings::setUint(SettingName idx, uint newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->settings[idx].val.numeric = newValue;
}

void Settings::setBool(SettingName idx, bool newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->settings[idx].val.logic = newValue;
}

void Settings::setScreenCorner(SettingName idx, ScreenCorner newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->settings[idx].val.enumScreenCorner = newValue;
}
