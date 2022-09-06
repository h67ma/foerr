#include <fstream>
#include <json/writer.h>
#include <SFML/Graphics.hpp>
#include "settings_manager.hpp"
#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

SettingsManager::SettingsManager()
{
	// window
	this->settings[SETT_FULLSCREEN_ENABLED].setupBool("FullscreenEnabled", false);
	this->settings[SETT_FPS_LIMIT_ENABLED].setupBool("FpsLimitEnabled", true);
	this->settings[SETT_FPS_LIMIT].setupUint("FpsLimit", 60);
	this->settings[SETT_FAKE_VSYNC_ENABLED].setupBool("FakeVsyncEnabled", false);
	// TODO? windowed size + position override

	// logging
	this->settings[SETT_WRITE_LOG_TO_FILE].setupBool("WriteLogToFile", true);

	// hud
	this->settings[SETT_PREFER_CUSTOM_CURSOR].setupBool("PreferCustomCursor", true);
	this->settings[SETT_SHOW_FPS_COUNTER].setupBool("ShowFpsCounter", true);
	this->settings[SETT_ANCHOR_LOG].setupScreenCorner("LogAnchor", CORNER_TOP_RIGHT);
	this->settings[SETT_ANCHOR_FPS].setupScreenCorner("FpsAnchor", CORNER_TOP_LEFT);
	this->settings[SETT_GUI_SCALE].setupGuiScale("GuiScale", GUI_NORMAL);
	this->settings[SETT_HUD_COLOR].setupColor("HudColor", sf::Color(0, 255, 153)); // default is greenish, same as in Remains
	//this->settings[SETT_LOG_MSG_TIMEOUT].setup("LogMsgTimeout", 3); // is this really necessary?

	// audio
	this->settings[SETT_FX_VOLUME].setupUint("FXVolume", 100, [](uint val){ return val <= 100; }); // 100 is max volume

	// debug
	// TODO maybe we could save window w&h on program exit and then restore it?
	this->settings[SETT_WINDOW_WIDTH].setupUint("WindowW", 1280, [](uint val){ return val <= 7680; }); // let's be realistic about max value
	this->settings[SETT_WINDOW_HEIGHT].setupUint("WindowH", 720, [](uint val){ return val <= 7680; }); // let's be realistic about max value
	this->settings[SETT_PRINT_MSGS].setupBool("PrintMsgs", false);
	this->settings[SETT_VERBOSE_DEBUG].setupBool("VerboseDebug", false);
	// TODO this->settings[SETT_SHOW_BOUNDING_BOXEN].setupBool("ShowBoundingBoxen", false); // Brian, what's the plural form of "box"?
	// TODO this->settings[SETT_DEV_CONSOLE_ENABLED].setupBool("DevConsoleEnabled", false);
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
