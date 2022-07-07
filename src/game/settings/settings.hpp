#pragma once

#include "../hud/hud.hpp"
#include "setting.hpp"
#include "../consts.h"

enum SettingName
{
	SETT_FULLSCREEN_ENABLED,
	SETT_FPS_LIMIT_ENABLED,
	SETT_FPS_LIMIT,
	SETT_FAKE_VSYNC_ENABLED,
	SETT_WRITE_LOG_TO_FILE,
	SETT_PREFER_CUSTOM_CURSOR,
	SETT_SHOW_FPS_COUNTER,
	SETT_ANCHOR_LOG,
	SETT_WINDOW_WIDTH,
	SETT_WINDOW_HEIGHT,
	SETT_DISPLAY_DEBUG_MSGS_IN_LOG,
	_SETTINGS_CNT // must be the last element
};

/**
 * The Settings class is a "live" representation of settings.
 * All changes to settings are temporary, unless they are written to the settings file.
 */
class Settings
{
	private:
		Setting settings[_SETTINGS_CNT];
	public:
		Settings();
		void saveConfig();
		void loadConfig();
		uint getUint(SettingName idx);
		bool getBool(SettingName idx);
		ScreenCorner getScreenCorner(SettingName idx);
		void setUint(SettingName idx, uint newValue);
		void setBool(SettingName idx, bool newValue);
		void setScreenCorner(SettingName idx, ScreenCorner newValue);
};
