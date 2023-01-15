#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <SFML/Graphics/Color.hpp>
#include "../hud/hud.hpp"
#include "setting.hpp"
#include "../consts.hpp"

enum SettingName
{
	SETT_FULLSCREEN_ENABLED,
	SETT_FPS_LIMIT_ENABLED,
	SETT_FPS_LIMIT,
	SETT_FAKE_VSYNC_ENABLED,
	SETT_ROOM_TRANSITION_DURATION_MS,
	SETT_WRITE_LOG_TO_FILE,
	SETT_PREFER_CUSTOM_CURSOR,
	SETT_SHOW_FPS_COUNTER,
	SETT_ANCHOR_LOG,
	SETT_ANCHOR_FPS,
	SETT_GUI_SCALE,
	SETT_HUD_COLOR,
	SETT_PAUSE_ON_FOCUS_LOSS,
	SETT_FX_VOLUME,
	SETT_AA,
	SETT_WINDOW_WIDTH,
	SETT_WINDOW_HEIGHT,
	SETT_AUTOLOAD_CAMPAIGN,
	SETT_PRINT_MSGS,
	SETT_VERBOSE_DEBUG,
	SETT_DEBUG_NAVIGATION, // TODO this should also include `testmode` (i.e. ability to travel to any location). should be settable only via command line/settings file editing
};

/**
 * The SettingsManager class is a "live" representation of settings.
 * All changes to settings are temporary, unless they are written to the settings file.
 *
 * Current strategy:
 * At the start, we initialize various objects with settings read from the SettingsManager
 * object. When settings change, main calls the proper update methods for all objects
 * which should adjust their behaviour/display according to the new settings (new values
 * passed as args, or via a ref to SettingsManager).
 *
 * Alternate strategy:
 * Objects which rely on various settings could register a callback to SettingsManager
 * (callbacks would be stored in a vector), the callback being responsible for updating
 * the object on settings change. We could then simply call all these callbacks from
 * inside SettingsManager.
 *
 * Settings file structure:
 * {
 *	"api_version": 1,
 *	"settings": {
 *		"setting_key": "value",
 *		"key2": 42,
 *		...
 *	}
 * }
 */
class SettingsManager
{
	private:
		static std::unordered_map<SettingName, std::unique_ptr<Setting>> settings;

		// SettingsManager is a kinda dumb place to put paths, but can't think of a better place rn.
		// paths are not stored in the main settings vector. they are instead generated based on user home dir,
		// with ::generatePathsAndMkdir()
		static std::string gameRootDir;
		static std::string saveDir;

	public:
		static void setup();
		static void saveConfig();
		static void loadConfig();
		static uint getUint(SettingName idx);
		static bool getBool(SettingName idx);
		static sf::Color getColor(SettingName idx);
		static ScreenCorner getScreenCorner(SettingName idx);
		static GuiScale getGuiScale(SettingName idx);
		static std::string getText(SettingName idx);
		static void setUint(SettingName idx, uint newValue);
		static void setBool(SettingName idx, bool newValue);
		static void setColor(SettingName idx, sf::Color newValue);
		static void setScreenCorner(SettingName idx, ScreenCorner newValue);
		static void setGuiScale(SettingName idx, GuiScale newValue);
		static void setText(SettingName idx, const std::string &newValue);
		static bool generatePathsAndMkdir();
		static std::string getGameRootDir();
		static std::string getSaveDir();
};
