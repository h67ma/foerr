#pragma once

#include "gui.hpp"
#include "setting.hpp"

enum SettingName
{
	SETT_FULLSCREEN_ENABLED,
	SETT_FPS_LIMIT_ENABLED,
	SETT_FPS_LIMIT,
	SETT_FAKE_VSYNC_ENABLED,
	SETT_WINDOW_WIDTH,
	SETT_WINDOW_HEIGHT,
	SETT_DISPLAY_DEBUG_MSGS_IN_LOG,
	SETT_WRITE_DEBUG_LOG_TO_FILE,
	SETT_NORMAL_FONT_SIZE,
	SETT_NORMAL_FONT_SIZE_WITH_GAP,
	SETT_SMALL_FONT_SIZE,
	SETT_PREFER_CUSTOM_CURSOR,
	SETT_SHOW_FPS_COUNTER,
	SETT_ANCHOR_LOG,
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
