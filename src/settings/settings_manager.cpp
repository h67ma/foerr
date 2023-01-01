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
#include "logic_setting.hpp"
#include "numeric_setting.hpp"
#include "color_setting.hpp"
#include "text_setting.hpp"
#include "guiscale_setting.hpp"
#include "screencorner_setting.hpp"

#define SANE_MAX_RESOLUTION 7680

#ifdef __linux__
	// currently custom cursor (with color support) is broken on linux
#define CUSTOM_CURSOR_DEF false
#else
#define CUSTOM_CURSOR_DEF true
#endif /* __linux__ */

// TODO should we care about this cpplint warning?
std::string SettingsManager::gameRootDir;
std::string SettingsManager::saveDir;

SettingsManager::SettingsManager()
{
	// +++++ window +++++

	this->settings.emplace(SETT_FULLSCREEN_ENABLED, std::make_unique<LogicSetting>("fullscreen_enabled", false));
	this->settings.emplace(SETT_FPS_LIMIT_ENABLED, std::make_unique<LogicSetting>("fps_limit_enabled", true));
	this->settings.emplace(SETT_FPS_LIMIT, std::make_unique<NumericSetting>("fps_limit", 60));
	this->settings.emplace(SETT_FAKE_VSYNC_ENABLED, std::make_unique<LogicSetting>("fake_vsync_enabled", false));
	// TODO? windowed size + position override

	// 0 = disable transition effect
	this->settings.emplace(SETT_ROOM_TRANSITION_DURATION_MS,
						   std::make_unique<NumericSetting>("room_transition_duration_ms", 200, [](uint val){
			return val <= 5000;
		}, "at most 5000ms"));

	// +++++ hud +++++

	this->settings.emplace(SETT_PREFER_CUSTOM_CURSOR, std::make_unique<LogicSetting>("prefer_custom_cursor",
																					 CUSTOM_CURSOR_DEF));
	this->settings.emplace(SETT_SHOW_FPS_COUNTER, std::make_unique<LogicSetting>("show_fps_counter", true));
	this->settings.emplace(SETT_ANCHOR_LOG, std::make_unique<ScreenCornerSetting>("log_anchor", CORNER_TOP_RIGHT));
	this->settings.emplace(SETT_ANCHOR_FPS, std::make_unique<ScreenCornerSetting>("fps_anchor", CORNER_TOP_LEFT));
	this->settings.emplace(SETT_GUI_SCALE, std::make_unique<GuiScaleSetting>("gui_scale", GUI_NORMAL));

	// default is greenish, same as in Remains
	this->settings.emplace(SETT_HUD_COLOR, std::make_unique<ColorSetting>("hud_color", sf::Color(0, 255, 153)));

	this->settings.emplace(SETT_PAUSE_ON_FOCUS_LOSS, std::make_unique<LogicSetting>("pause_on_focus_loss", true));

	// TODO is this really necessary?
	//this->settings.emplace(SETT_LOG_MSG_TIMEOUT, std::make_unique<NumericSetting>("log_msg_timeout", 3));

	// +++++ audio +++++

	// 100 is max volume
	this->settings.emplace(SETT_FX_VOLUME, std::make_unique<NumericSetting>("fx_volume", 100, [](uint val){
		return val <= 100;
	}, "between 0 and 100"));

	// +++++ video +++++

	this->settings.emplace(SETT_AA, std::make_unique<NumericSetting>("antialiasing", 8, [](uint val){
		return val % 2 == 0 && val <= sf::RenderTexture::getMaximumAntialiasingLevel();
	}, litSprintf("a power of 2, between 0 and %d", sf::RenderTexture::getMaximumAntialiasingLevel())));

	// TODO maybe we could save window w&h on program exit and then restore it?
	// let's be realistic about max window size
	this->settings.emplace(SETT_WINDOW_WIDTH, std::make_unique<NumericSetting>("window_w", 1280, [](uint val){
		return val <= SANE_MAX_RESOLUTION;
	}, litSprintf("between 0 and %d", SANE_MAX_RESOLUTION)));
	this->settings.emplace(SETT_WINDOW_HEIGHT, std::make_unique<NumericSetting>("window_h", 720, [](uint val){
		return val <= SANE_MAX_RESOLUTION;
	}, litSprintf("between 0 and %d", SANE_MAX_RESOLUTION)));

	// +++++ debug +++++

	// "" = do not autoload
	this->settings.emplace(SETT_AUTOLOAD_CAMPAIGN, std::make_unique<TextSetting>("autoload_campaign", ""));

	this->settings.emplace(SETT_WRITE_LOG_TO_FILE, std::make_unique<LogicSetting>("write_log_to_file", true));
	this->settings.emplace(SETT_PRINT_MSGS, std::make_unique<LogicSetting>("print_msgs_cout", false));
	this->settings.emplace(SETT_VERBOSE_DEBUG, std::make_unique<LogicSetting>("verbose_debug", false));
	// TODO this->settings.emplace(SETT_SHOW_BOUNDING_BOXEN, std::make_unique<LogicSetting>(("show_bounding_boxen", false));
	// TODO this->settings.emplace(SETT_DEV_CONSOLE_ENABLED, std::make_unique<LogicSetting>(("dev_console_enabled", false));
}

void SettingsManager::saveConfig()
{
	json root;

	root.emplace(FOERR_JSON_API_VERSION, JSON_API_VERSION);

	json keysNode;
	for (auto &sett : this->settings)
	{
		keysNode.emplace(sett.second->getKey(), sett.second->getJsonValue());
	}

	root.emplace(FOERR_JSON_KEY_SETTINGS, keysNode);

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

	auto keysSearch = root.find(FOERR_JSON_KEY_SETTINGS);
	if (keysSearch == root.end())
	{
		Log::w(STR_MISSING_KEY, PATH_SETTINGS, FOERR_JSON_KEY_SETTINGS);
		return;
	}

	if (!keysSearch->is_object())
	{
		Log::w(STR_INVALID_TYPE, PATH_SETTINGS, FOERR_JSON_KEY_SETTINGS);
		return;
	}

	for (auto &sett : this->settings)
	{
		auto search = keysSearch->find(sett.second->getKey());
		if (search == keysSearch->end())
		{
			Log::w(STR_SETTINGS_KEY_MISSING, sett.second->getKey().c_str());
			continue;
		}

		try
		{
			sett.second->loadFromJson(search.value());
		}
		catch (const json::type_error &ex)
		{
			Log::w(STR_INVALID_TYPE_EX, path.c_str(), sett.second->getKey().c_str(), ex.what());
		}
	}
}

uint SettingsManager::getUint(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return 0; // default value is better than crash

	return search->second->val.numeric;
}

bool SettingsManager::getBool(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return false; // default value is better than crash

	return search->second->val.logic;
}

sf::Color SettingsManager::getColor(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return sf::Color::White; // default value is better than crash

	return sf::Color(search->second->val.numeric);
}

ScreenCorner SettingsManager::getScreenCorner(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return CORNER_TOP_LEFT; // default value is better than crash

	return search->second->val.enumScreenCorner;
}

GuiScale SettingsManager::getGuiScale(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return GUI_NORMAL; // default value is better than crash

	return search->second->val.guiScale;
}

std::string SettingsManager::getText(SettingName name)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
		return ""; // default value is better than crash

	return search->second->textVal;
}

void SettingsManager::setUint(SettingName name, uint newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second->val.numeric = newValue;
}

void SettingsManager::setBool(SettingName name, bool newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second->val.logic = newValue;
}

void SettingsManager::setColor(SettingName name, sf::Color newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second->val.numeric = newValue.toInteger();
}

void SettingsManager::setScreenCorner(SettingName name, ScreenCorner newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second->val.enumScreenCorner = newValue;
}

void SettingsManager::setText(SettingName name, std::string newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second->textVal = newValue;
}

void SettingsManager::setGuiScale(SettingName name, GuiScale newValue)
{
	auto search = this->settings.find(name);
	if (search == this->settings.end())
	{
		Log::w(STR_SETT_NOT_PRESENT, name);
		return;
	}

	search->second->val.guiScale = newValue;
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
