#pragma once

#include <sfml/Graphics.hpp>
#include "../hud/hud.hpp"
#include "setting.hpp"
#include "../consts.hpp"

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
	SETT_ANCHOR_FPS,
	SETT_GUI_SCALE,
	SETT_HUD_COLOR,
	SETT_WINDOW_WIDTH,
	SETT_WINDOW_HEIGHT,
	SETT_PRINT_MSGS,
	SETT_VERBOSE_DEBUG,
	_SETTINGS_CNT
};

/**
 * The SettingsManager class is a "live" representation of settings.
 * All changes to settings are temporary, unless they are written to the settings file.
 */
class SettingsManager
{
	private:
		Setting settings[_SETTINGS_CNT];
	public:
		SettingsManager();
		void saveConfig();
		void loadConfig();
		uint getUint(SettingName idx);
		bool getBool(SettingName idx);
		sf::Color getColor(SettingName idx);
		ScreenCorner getScreenCorner(SettingName idx);
		GuiScale getGuiScale(SettingName idx);
		void setUint(SettingName idx, uint newValue);
		void setBool(SettingName idx, bool newValue);
		void setColor(SettingName idx, sf::Color newValue);
		void setScreenCorner(SettingName idx, ScreenCorner newValue);
		void setGuiScale(SettingName idx, GuiScale newValue);
};
