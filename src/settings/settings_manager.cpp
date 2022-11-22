#include "settings_manager.hpp"
#ifdef __linux__
#include <unistd.h>
#include <pwd.h>
#endif /* __linux__ */
#include <string>
#include <filesystem>
#include <SFML/Graphics/RenderTexture.hpp>
#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/json.hpp"
#include "../util/util.hpp"

#define SANE_MAX_RESOLUTION 7680

// TODO should we care about this cpplint warning?
std::string SettingsManager::gameRootDir;
std::string SettingsManager::saveDir;

SettingsManager::SettingsManager() :
	settings {
		// +++++ window +++++

		{ SETT_FULLSCREEN_ENABLED, Setting("fullscreen_enabled", false) },
		{ SETT_FPS_LIMIT_ENABLED, Setting("fps_limit_enabled", true) },
		{ SETT_FPS_LIMIT, Setting("fps_limit", 60U) },
		{ SETT_FAKE_VSYNC_ENABLED, Setting("fake_vsync_enabled", false) },
		// TODO? windowed size + position override

		// +++++ hud +++++

#ifdef __linux__
	// currently custom cursor (with color support) is broken on linux
	{ SETT_PREFER_CUSTOM_CURSOR, Setting("prefer_custom_cursor", false) },
#else
	{ SETT_PREFER_CUSTOM_CURSOR, Setting("prefer_custom_cursor", true) },
#endif /* __linux__ */

		{ SETT_SHOW_FPS_COUNTER, Setting("show_fps_counter", true) },
		{ SETT_ANCHOR_LOG, Setting("log_anchor", CORNER_TOP_RIGHT) },
		{ SETT_ANCHOR_FPS, Setting("fps_anchor", CORNER_TOP_LEFT) },
		{ SETT_GUI_SCALE, Setting("gui_scale", GUI_NORMAL) },

		// default is greenish, same as in Remains
		{ SETT_HUD_COLOR, Setting("hud_color", sf::Color(0, 255, 153)) },

		//{ SETT_LOG_MSG_TIMEOUT].setup("log_msg_timeout", 3) }, // is this really necessary?

		// +++++ audio +++++

		// 100 is max volume
		{ SETT_FX_VOLUME, Setting("fx_volume", 100U, [](uint val){ return val <= 100; }, "between 0 and 100") },

		// +++++ video +++++

		{ SETT_AA, Setting("antialiasing", 8U, [](uint val){
			return val % 2 == 0 && val <= sf::RenderTexture::getMaximumAntialiasingLevel();
		}, litSprintf("a power of 2, between 0 and %d", sf::RenderTexture::getMaximumAntialiasingLevel())) },

		// TODO maybe we could save window w&h on program exit and then restore it?
		// let's be realistic about max window size
		{ SETT_WINDOW_WIDTH, Setting("window_w", 1280U, [](uint val){ return val <= SANE_MAX_RESOLUTION; },
			litSprintf("between 0 and %d", SANE_MAX_RESOLUTION)) },
		{ SETT_WINDOW_HEIGHT, Setting("window_h", 720U, [](uint val){ return val <= SANE_MAX_RESOLUTION; },
			litSprintf("between 0 and %d", SANE_MAX_RESOLUTION)) },

		// +++++ debug +++++

		{ SETT_WRITE_LOG_TO_FILE, Setting("write_log_to_file", true) },
		{ SETT_PRINT_MSGS, Setting("print_msgs_cout", false) },
		{ SETT_VERBOSE_DEBUG, Setting("verbose_debug", false) }
		// TODO { SETT_SHOW_BOUNDING_BOXEN, Setting(("show_bounding_boxen", false) },
		// TODO { SETT_DEV_CONSOLE_ENABLED, Setting(("dev_console_enabled", false) },
	}
{
	// J'zargo has designed it to be an excellent flame cloak, with a twist
}

void SettingsManager::saveConfig()
{
	json root;

	for (auto &sett : this->settings)
	{
		root.emplace(sett.second.getKey(), sett.second.getJsonValue());
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
		auto search = root.find(sett.second.getKey());
		if (search == root.end())
		{
			Log::w(STR_SETTINGS_KEY_MISSING, path.c_str(), sett.second.getKey().c_str());
			continue;
		}

		try
		{
			sett.second.loadFromJson(search.value());
		}
		catch (const json::type_error &ex)
		{
			Log::w(STR_INVALID_TYPE_EX, path.c_str(), sett.second.getKey().c_str(), ex.what());
		}
	}
}

uint SettingsManager::getUint(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return 0; // default value is better than crash

	return search->second.val.numeric;
}

bool SettingsManager::getBool(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return false; // default value is better than crash

	return search->second.val.logic;
}

sf::Color SettingsManager::getColor(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return sf::Color::White; // default value is better than crash

	return sf::Color(search->second.val.numeric);
}

ScreenCorner SettingsManager::getScreenCorner(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return CORNER_TOP_LEFT; // default value is better than crash

	return search->second.val.enumScreenCorner;
}

GuiScale SettingsManager::getGuiScale(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return GUI_NORMAL; // default value is better than crash

	return search->second.val.guiScale;
}

void SettingsManager::setUint(SettingName name, uint newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second.val.numeric = newValue;
}

void SettingsManager::setBool(SettingName name, bool newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second.val.logic = newValue;
}

void SettingsManager::setColor(SettingName name, sf::Color newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second.val.numeric = newValue.toInteger();
}

void SettingsManager::setScreenCorner(SettingName name, ScreenCorner newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second.val.enumScreenCorner = newValue;
}

void SettingsManager::setGuiScale(SettingName name, GuiScale newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second.val.guiScale = newValue;
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
	homeDir = getenv("USERPROFILE");
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
