#include "settings_manager.hpp"
#ifdef __linux__
#include <unistd.h>
#include <pwd.h>
#endif /* __linux__ */
#include <string>
#include <filesystem>
#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/json.hpp"

// TODO should we care about this cpplint warning?
std::string SettingsManager::gameRootDir;
std::string SettingsManager::saveDir;

SettingsManager::SettingsManager()
{
	this->settings.resize(_SETTINGS_CNT);

	// +++++ window +++++

	this->settings[SETT_FULLSCREEN_ENABLED].setupBool("fullscreen_enabled", false);
	this->settings[SETT_FPS_LIMIT_ENABLED].setupBool("fps_limit_enabled", true);
	this->settings[SETT_FPS_LIMIT].setupUint("fps_limit", 60);
	this->settings[SETT_FAKE_VSYNC_ENABLED].setupBool("fake_vsync_enabled", false);
	// TODO? windowed size + position override

	// +++++ hud +++++

#ifdef __linux__
	// currently custom cursor (with color support) is broken on linux
	this->settings[SETT_PREFER_CUSTOM_CURSOR].setupBool("prefer_custom_cursor", false);
#else
	this->settings[SETT_PREFER_CUSTOM_CURSOR].setupBool("prefer_custom_cursor", true);
#endif /* __linux__ */

	this->settings[SETT_SHOW_FPS_COUNTER].setupBool("show_fps_counter", true);
	this->settings[SETT_ANCHOR_LOG].setupScreenCorner("log_anchor", CORNER_TOP_RIGHT);
	this->settings[SETT_ANCHOR_FPS].setupScreenCorner("fps_anchor", CORNER_TOP_LEFT);
	this->settings[SETT_GUI_SCALE].setupGuiScale("gui_scale", GUI_NORMAL);

	// default is greenish, same as in Remains
	this->settings[SETT_HUD_COLOR].setupColor("hud_color", sf::Color(0, 255, 153));

	//this->settings[SETT_LOG_MSG_TIMEOUT].setup("log_msg_timeout", 3); // is this really necessary?

	// +++++ audio +++++

	// 100 is max volume
	this->settings[SETT_FX_VOLUME].setupUint("fx_volume", 100, [](uint val){ return val <= 100; });

	// +++++ debug +++++
	// TODO maybe we could save window w&h on program exit and then restore it?

	// let's be realistic about max window size
	this->settings[SETT_WINDOW_WIDTH].setupUint("window_w", 1280, [](uint val){ return val <= 7680; });
	this->settings[SETT_WINDOW_HEIGHT].setupUint("window_h", 720, [](uint val){ return val <= 7680; });

	this->settings[SETT_WRITE_LOG_TO_FILE].setupBool("write_log_to_file", true);
	this->settings[SETT_PRINT_MSGS].setupBool("print_msgs_cout", false);
	this->settings[SETT_VERBOSE_DEBUG].setupBool("verbose_debug", false);
	// TODO this->settings[SETT_SHOW_BOUNDING_BOXEN].setupBool("show_bounding_boxen", false);
	// TODO this->settings[SETT_DEV_CONSOLE_ENABLED].setupBool("dev_console_enabled", false);
}

void SettingsManager::saveConfig()
{
	json root;

	for (auto &sett : this->settings)
	{
		root.emplace(sett.getKey(), sett.getJsonValue());
	}

	writeJsonToFile(root, pathCombine(this->gameRootDir, PATH_SETTINGS));
	Log::i("Saved settings");
}

void SettingsManager::loadConfig()
{
	json root;
	std::string path = pathCombine(this->gameRootDir, PATH_SETTINGS);

	if (!loadJsonFromFile(root, path, true))
	{
		// we'll just run on default settings (which are already assigned)
		Log::d(STR_SETTINGS_OPEN_ERROR);
		return;
	}

	for (auto &sett : this->settings)
	{
		auto search = root.find(sett.getKey());
		if (search == root.end())
		{
			Log::w(STR_SETTINGS_KEY_MISSING, path.c_str(), sett.getKey().c_str());
			continue;
		}

		try
		{
			sett.loadFromJson(search.value());
		}
		catch (const json::type_error &ex)
		{
			Log::w(STR_INVALID_TYPE_EX, path.c_str(), sett.getKey().c_str(), ex.what());
		}
	}
}

uint SettingsManager::getUint(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return 0; // default value is better than crash

	return this->settings[idx].val.numeric;
}

bool SettingsManager::getBool(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return false; // default value is better than crash

	return this->settings[idx].val.logic;
}

sf::Color SettingsManager::getColor(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return sf::Color::White; // default value is better than crash

	return sf::Color(this->settings[idx].val.numeric);
}

ScreenCorner SettingsManager::getScreenCorner(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return CORNER_TOP_LEFT; // default value is better than crash

	return this->settings[idx].val.enumScreenCorner;
}

GuiScale SettingsManager::getGuiScale(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return GUI_NORMAL; // default value is better than crash

	return this->settings[idx].val.guiScale;
}

void SettingsManager::setUint(SettingName idx, uint newValue)
{
	if (idx >= _SETTINGS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS_SETT_NOT_SET);
		return;
	}

	this->settings[idx].val.numeric = newValue;
}

void SettingsManager::setBool(SettingName idx, bool newValue)
{
	if (idx >= _SETTINGS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS_SETT_NOT_SET);
		return;
	}

	this->settings[idx].val.logic = newValue;
}

void SettingsManager::setColor(SettingName idx, sf::Color newValue)
{
	if (idx >= _SETTINGS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS_SETT_NOT_SET);
		return;
	}

	this->settings[idx].val.numeric = newValue.toInteger();
}

void SettingsManager::setScreenCorner(SettingName idx, ScreenCorner newValue)
{
	if (idx >= _SETTINGS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS_SETT_NOT_SET);
		return;
	}

	this->settings[idx].val.enumScreenCorner = newValue;
}

void SettingsManager::setGuiScale(SettingName idx, GuiScale newValue)
{
	if (idx >= _SETTINGS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS_SETT_NOT_SET);
		return;
	}

	this->settings[idx].val.guiScale = newValue;
}

/**
 * @brief Generates game root and savegame dirs and creates directories for both.
 *
 * Can be called before calling ::loadConfig(), as paths do not depend on any custom config (it would be impossible, as
 * config.json is stored *inside* game root dir).
 *
 * @return true if paths are valid and mkdir succeeded
 * @return false if paths are invalid or mkdir failed
 */
bool SettingsManager::generatePathsAndMkdir()
{
	const char* homeDir;
#ifdef __linux__
	homeDir = getenv("HOME");
	if (homeDir == NULL)
		homeDir = getpwuid(getuid())->pw_dir;
#elif defined(_WIN32)
	homeDir = getenv("USERPROFILE"));
#else // TODO add a case for macos
	homedir = "."; // I guess just store files in current directory
#endif

	if (!std::filesystem::exists(homeDir))
		homeDir = "."; // fallback to current dir

	SettingsManager::gameRootDir = pathCombine(homeDir, PATH_DIR_GAMEDATA);
	SettingsManager::saveDir = pathCombine(gameRootDir, PATH_DIR_SAVES);

	if (std::filesystem::exists(saveDir))
		return true;

	if (std::filesystem::create_directories(saveDir))
	{
		Log::d(STR_CREATED_GAME_DIRS);
		return true;
	}

	return false;
}

std::string SettingsManager::getGameRootDir()
{
	return SettingsManager::gameRootDir;
}

std::string SettingsManager::getSaveDir()
{
	return SettingsManager::saveDir;
}
