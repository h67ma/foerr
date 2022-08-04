#include <fstream>
#include <json/writer.h>
#include "settings_manager.hpp"
#include "../consts.h"
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
	//this->settings[SETT_LOG_MSG_TIMEOUT].setup("LogMsgTimeout", 3); // is this really necessary?
	// TODO this->settings[SETT_FONT_SCALING_FACTOR].setup("FontScalingFactor", 1.0);

	// debug
	// TODO maybe we could save window w&h on program exit and then restore it?
	this->settings[SETT_WINDOW_WIDTH].setup("WindowW", 1280U, true);
	this->settings[SETT_WINDOW_HEIGHT].setup("WindowH", 720U, true);
	this->settings[SETT_DISPLAY_DEBUG_MSGS_IN_LOG].setup("DisplayDebugMsgsInLog", false, true);
	// TODO this->settings[SETT_SHOW_BOUNDING_BOXEN].setup("ShowBoundingBoxen", false, true); // Brian, what's the plural form of "box"?
	// TODO this->settings[SETT_DEV_CONSOLE_ENABLED].setup("DevConsoleEnabled", false, true);
}

void SettingsManager::saveConfig()
{
	Json::Value root;

	for (size_t i = 0; i < _SETTINGS_CNT; i++)
	{
		Setting *sett = &this->settings[i];

		if (sett->isDebug())
			continue; // don't write debug settings

		root[sett->getKey().c_str()] = sett->getJsonValue();
	}

	std::ofstream file(PATH_SETTINGS);
	file << root << std::endl;
	file.close();

	Log::logStderr(LOG_DEBUG, "Saved settings");
}

void SettingsManager::loadConfig()
{
	std::ifstream file(PATH_SETTINGS, std::ifstream::binary);
	Json::Value root;

	if (!file.is_open())
		return; // we'll just run on default settings (which are already assigned)

	file >> root;
	file.close();

	for (size_t i = 0; i < _SETTINGS_CNT; i++)
	{
		Setting *sett = &this->settings[i];

		// TODO C related mystery for another time:
		// why can't we do `key = sett->getKey().c_str();` and use it as a variable later?

		if (!root.isMember(sett->getKey().c_str()))
		{
			// settings file is missing this key

			// debug settings are missing "by default" so we shouldn't warn user
			if (!sett->isDebug())
				Log::logStderr(LOG_WARNING, STR_SETTING_LOAD_FAIL, sett->getKey().c_str());

			continue;
		}

		sett->loadFromJson(root[sett->getKey().c_str()]);
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

void SettingsManager::setUint(SettingName idx, uint newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->settings[idx].val.numeric = newValue;
}

void SettingsManager::setBool(SettingName idx, bool newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->settings[idx].val.logic = newValue;
}

void SettingsManager::setScreenCorner(SettingName idx, ScreenCorner newValue)
{
	if (idx >= _SETTINGS_CNT)
		return; // TODO print some error or something

	this->settings[idx].val.enumScreenCorner = newValue;
}
