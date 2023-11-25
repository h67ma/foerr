// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "settings_manager.hpp"

#ifdef __linux__
#include <pwd.h>
#include <unistd.h>
#endif /* __linux__ */

#include <filesystem>
#include <string>

#include <SFML/Graphics/RenderTexture.hpp>

#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/json.hpp"
#include "../util/util.hpp"
#include "color_setting.hpp"
#include "enum_setting.hpp"
#include "float_setting.hpp"
#include "logic_setting.hpp"
#include "numeric_setting.hpp"
#include "text_setting.hpp"

#define SETT_SETUP(type, name, def) \
	SettingsManager::settings.emplace_back(std::make_unique<type>(#name, SettingsManager::name, def))

#define SETT_SETUP_ENUM(type, name, def, maxValue) \
	SettingsManager::settings.emplace_back(std::make_unique<type>(#name, SettingsManager::name, def, maxValue))

#define SETT_SETUP_ENUM_SCREENCORNER(name, def) SETT_SETUP_ENUM(EnumSetting<ScreenCorner>, name, def, _CORNER_CNT)
#define SETT_SETUP_ENUM_SCREENSIDE(name, def) SETT_SETUP_ENUM(EnumSetting<ScreenSide>, name, def, _SIDE_CNT)

#define SETT_SETUP_CONSTR(type, name, def, constraint, hint) \
	SettingsManager::settings.emplace_back(std::make_unique<type>(#name, SettingsManager::name, def, constraint, hint))

#define MAX_RESOLUTION 7680 // let's be realistic about max window size
#define MAX_VOLUME 100
#define MAX_ROOM_TRANSITION_MS 5000

#define DEFAULT_AA 8

std::string SettingsManager::gameRootDir;
std::string SettingsManager::saveDir;

std::vector<std::unique_ptr<Setting>> SettingsManager::settings;

///// window /////
bool SettingsManager::fullscreen;
bool SettingsManager::fpsLimitEnabled;
uint SettingsManager::fpsLimit;
bool SettingsManager::fakeVsync;
uint SettingsManager::roomTransitionDurationMs;

///// hud /////
bool SettingsManager::preferCustomCursor;
bool SettingsManager::showFpsCounter;
ScreenCorner SettingsManager::logAnchor;
ScreenCorner SettingsManager::fpsAnchor;
float SettingsManager::guiScale;
SerializableColor SettingsManager::hudColor;
bool SettingsManager::pauseOnFocusLoss;

///// audio /////
uint SettingsManager::fxVolume;

///// video /////
uint SettingsManager::antiAliasing;
uint SettingsManager::windowWidth;
uint SettingsManager::windowHeight;

///// debug /////
std::string SettingsManager::debugAutoloadCampaign;
bool SettingsManager::debugWriteLogToFile;
bool SettingsManager::debugPrintToStderr;
bool SettingsManager::debugVerbose;
bool SettingsManager::debugNavigation;
bool SettingsManager::debugBoundingBoxes;
ScreenSide SettingsManager::debugConsoleAnchor;
bool SettingsManager::debugConsoleEnabled;

void SettingsManager::setup()
{
	///// window /////

	SETT_SETUP(LogicSetting, fullscreen, false);
	SETT_SETUP(LogicSetting, fpsLimitEnabled, true);
	SETT_SETUP(NumericSetting, fpsLimit, 60);
	SETT_SETUP(LogicSetting, fakeVsync, false);
	// TODO? windowed size + position override

	// 0 = disable transition effect
	SETT_SETUP_CONSTR(
		NumericSetting, roomTransitionDurationMs, 200, [](uint val) { return val <= MAX_ROOM_TRANSITION_MS; },
		"between 0 and " STR_EXP(MAX_ROOM_TRANSITION_MS) "ms");

	///// hud /////

	SETT_SETUP(LogicSetting, preferCustomCursor, true);
	SETT_SETUP(LogicSetting, showFpsCounter, true);
	SETT_SETUP_ENUM_SCREENCORNER(logAnchor, CORNER_TOP_RIGHT);
	SETT_SETUP_ENUM_SCREENCORNER(fpsAnchor, CORNER_TOP_LEFT);
	SETT_SETUP_CONSTR(
		FloatSetting, guiScale, 1.F, [](float val) { return val >= GUI_SCALE_MIN_VALUE && val <= GUI_SCALE_MAX_VALUE; },
		litSprintf("between %g and %g", GUI_SCALE_MIN_VALUE, GUI_SCALE_MAX_VALUE));
	SETT_SETUP(ColorSetting, hudColor, SerializableColor(0, 255, 153)); // default is greenish, same as in Remains
	SETT_SETUP(LogicSetting, pauseOnFocusLoss, true);
	// TODO? SETT_SETUP(NumericSetting, logMsgTimeoutSec, 3); // is this really necessary?

	///// audio /////

	SETT_SETUP_CONSTR(
		NumericSetting, fxVolume, MAX_VOLUME, [](uint val) { return val <= MAX_VOLUME; },
		"between 0 and " STR_EXP(MAX_VOLUME));

	///// video /////

	uint maxSupportedAA = sf::RenderTexture::getMaximumAntialiasingLevel();
	SETT_SETUP_CONSTR(
		NumericSetting, antiAliasing, DEFAULT_AA > maxSupportedAA ? maxSupportedAA : DEFAULT_AA,
		[maxSupportedAA](uint val) { return val % 2 == 0 && val <= maxSupportedAA; },
		litSprintf("a power of 2, between 0 and %d", maxSupportedAA));

	// TODO maybe we could save window w&h on program exit and then restore it?
	SETT_SETUP_CONSTR(
		NumericSetting, windowWidth, 1280, [](uint val) { return val > 0 && val <= MAX_RESOLUTION; },
		"between 0 and " STR_EXP(MAX_RESOLUTION));
	SETT_SETUP_CONSTR(
		NumericSetting, windowHeight, 720, [](uint val) { return val > 0 && val <= MAX_RESOLUTION; },
		"between 0 and " STR_EXP(MAX_RESOLUTION));

	///// debug /////

	SETT_SETUP(TextSetting, debugAutoloadCampaign, ""); // "" = do not autoload
	SETT_SETUP(LogicSetting, debugWriteLogToFile, true);
	SETT_SETUP(LogicSetting, debugPrintToStderr, false);
	SETT_SETUP(LogicSetting, debugVerbose, false);

	// TODO debugNavigation should also include `testmode` functionality (i.e. ability to travel to any location)
	SETT_SETUP(LogicSetting, debugNavigation, false);
	SETT_SETUP(LogicSetting, debugBoundingBoxes, false);
	SETT_SETUP_ENUM_SCREENSIDE(debugConsoleAnchor, SIDE_BOTTOM);
	SETT_SETUP(LogicSetting, debugConsoleEnabled, false);
}

void SettingsManager::saveConfig()
{
	nlohmann::json root;

	root.emplace(FOERR_JSON_API_VERSION, JSON_API_VERSION);

	nlohmann::json keysNode;
	for (const auto& sett : SettingsManager::settings)
	{
		keysNode.emplace(sett->getKey(), sett->getJsonValue());
	}

	root.emplace(FOERR_JSON_KEY_SETTINGS, keysNode);

	writeJsonToFile(root, pathCombine(SettingsManager::gameRootDir, PATH_SETTINGS));
	Log::i(STR_SETTINGS_SAVED);
}

void SettingsManager::loadConfig()
{
	nlohmann::json root;
	std::string path = pathCombine(SettingsManager::gameRootDir, PATH_SETTINGS);

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

	for (auto& sett : SettingsManager::settings)
	{
		auto search = keysSearch->find(sett->getKey());
		if (search == keysSearch->end())
		{
			Log::w(STR_SETTINGS_KEY_MISSING, sett->getKey().c_str(), sett->defaultToString().c_str());
			continue;
		}

		try
		{
			sett->loadFromJson(search.value());
		}
		catch (const nlohmann::json::type_error& ex)
		{
			Log::w(STR_INVALID_TYPE_EX, path.c_str(), sett->getKey().c_str(), ex.what());
		}
	}
}

void SettingsManager::resetAllToDefault()
{
	for (auto& sett : SettingsManager::settings)
	{
		sett->resetToDefault();
	}

	Log::i(STR_SETTINGS_RESETTED);
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
