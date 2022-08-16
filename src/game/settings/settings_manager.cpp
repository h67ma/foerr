#include <fstream>
#include <json/writer.h>
#include "settings_manager.hpp"
#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

SettingsManager::SettingsManager()
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
	this->settings[SETT_HUD_SCALE].setup("HudScale", HUD_NORMAL);
	//this->settings[SETT_LOG_MSG_TIMEOUT].setup("LogMsgTimeout", 3); // is this really necessary?

	// debug
	// TODO maybe we could save window w&h on program exit and then restore it?
	this->settings[SETT_WINDOW_WIDTH].setup("WindowW", 1280U);
	this->settings[SETT_WINDOW_HEIGHT].setup("WindowH", 720U);
	this->settings[SETT_PRINT_MSGS].setup("PrintMsgs", false);
	this->settings[SETT_VERBOSE_DEBUG].setup("VerboseDebug", false);
	// TODO this->settings[SETT_SHOW_BOUNDING_BOXEN].setup("ShowBoundingBoxen", false); // Brian, what's the plural form of "box"?
	// TODO this->settings[SETT_DEV_CONSOLE_ENABLED].setup("DevConsoleEnabled", false);
}

void SettingsManager::saveConfig()
{
	Json::Value root;

	for (size_t i = 0; i < _SETTINGS_CNT; i++)
	{
		Setting *sett = &this->settings[i];

		root[sett->getKey().c_str()] = sett->getJsonValue();
	}

	std::ofstream file(PATH_SETTINGS);
	file << root << std::endl;
	file.close();

	Log::i("Saved settings");
}

void SettingsManager::loadConfig()
{
	Json::Value root;

	if (!loadJsonFromFile(root, std::string(PATH_SETTINGS)))
	{
		// we'll just run on default settings (which are already assigned)
		Log::w(STR_SETTINGS_OPEN_ERROR);
		return;
	}

	for (size_t i = 0; i < _SETTINGS_CNT; i++)
	{
		Setting *sett = &this->settings[i];

		// TODO C related mystery for another time:
		// why can't we do `key = sett->getKey().c_str();` and use it as a variable later?

		if (!root.isMember(sett->getKey().c_str()))
		{
			// settings file is missing this key
			Log::w(STR_SETTINGS_KEY_MISSING, PATH_SETTINGS, sett->getKey().c_str());
			continue;
		}

		try
		{
			sett->loadFromJson(root[sett->getKey().c_str()]);
		}
		catch (const Json::LogicError &ex)
		{
			Log::w(STR_INVALID_TYPE_EX, PATH_SETTINGS, sett->getKey().c_str(), ex.what());
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

ScreenCorner SettingsManager::getScreenCorner(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return CORNER_TOP_LEFT; // default value is better than crash

	return this->settings[idx].val.enumScreenCorner;
}

HudScale SettingsManager::getHudScale(SettingName idx)
{
	if (idx >= _SETTINGS_CNT)
		return HUD_NORMAL; // default value is better than crash

	return this->settings[idx].val.hudScale;
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

void SettingsManager::setScreenCorner(SettingName idx, ScreenCorner newValue)
{
	if (idx >= _SETTINGS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS_SETT_NOT_SET);
		return;
	}

	this->settings[idx].val.enumScreenCorner = newValue;
}

void SettingsManager::setHudScale(SettingName idx, HudScale newValue)
{
	if (idx >= _SETTINGS_CNT)
	{
		Log::w(STR_IDX_OUTTA_BOUNDS_SETT_NOT_SET);
		return;
	}

	this->settings[idx].val.hudScale = newValue;
}
