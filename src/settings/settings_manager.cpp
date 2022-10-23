#include "settings_manager.hpp"
#include <string>
#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/json.hpp"

SettingsManager::SettingsManager()
{
	this->settings.resize(_SETTINGS_CNT);

	// window
	this->settings[SETT_FULLSCREEN_ENABLED].setupBool("fullscreen_enabled", false);
	this->settings[SETT_FPS_LIMIT_ENABLED].setupBool("fps_limit_enabled", true);
	this->settings[SETT_FPS_LIMIT].setupUint("fps_limit", 60);
	this->settings[SETT_FAKE_VSYNC_ENABLED].setupBool("fake_vsync_enabled", false);
	// TODO? windowed size + position override

	// hud
	this->settings[SETT_PREFER_CUSTOM_CURSOR].setupBool("prefer_custom_cursor", true);
	this->settings[SETT_SHOW_FPS_COUNTER].setupBool("show_fps_counter", true);
	this->settings[SETT_ANCHOR_LOG].setupScreenCorner("log_anchor", CORNER_TOP_RIGHT);
	this->settings[SETT_ANCHOR_FPS].setupScreenCorner("fps_anchor", CORNER_TOP_LEFT);
	this->settings[SETT_GUI_SCALE].setupGuiScale("gui_scale", GUI_NORMAL);
	this->settings[SETT_HUD_COLOR].setupColor("hud_color", sf::Color(0, 255, 153)); // default is greenish, same as in Remains
	//this->settings[SETT_LOG_MSG_TIMEOUT].setup("log_msg_timeout", 3); // is this really necessary?

	// audio
	this->settings[SETT_FX_VOLUME].setupUint("fx_volume", 100, [](uint val){ return val <= 100; }); // 100 is max volume

	// debug
	// TODO maybe we could save window w&h on program exit and then restore it?
	this->settings[SETT_WINDOW_WIDTH].setupUint("window_w", 1280, [](uint val){ return val <= 7680; }); // let's be realistic about max value
	this->settings[SETT_WINDOW_HEIGHT].setupUint("window_h", 720, [](uint val){ return val <= 7680; }); // let's be realistic about max value
	this->settings[SETT_WRITE_LOG_TO_FILE].setupBool("write_log_to_file", true);
	this->settings[SETT_PRINT_MSGS].setupBool("print_msgs_cout", false);
	this->settings[SETT_VERBOSE_DEBUG].setupBool("verbose_debug", false);
	// TODO this->settings[SETT_SHOW_BOUNDING_BOXEN].setupBool("show_bounding_boxen", false); // Brian, what's the plural form of "box"?
	// TODO this->settings[SETT_DEV_CONSOLE_ENABLED].setupBool("dev_console_enabled", false);
}

void SettingsManager::saveConfig()
{
	json root;

	for (auto &sett : this->settings)
	{
		root.emplace(sett.getKey(), sett.getJsonValue());
	}

	writeJsonToFile(root, PATH_SETTINGS);
	Log::i("Saved settings");
}

void SettingsManager::loadConfig()
{
	json root;

	if (!loadJsonFromFile(root, std::string(PATH_SETTINGS)))
	{
		// we'll just run on default settings (which are already assigned)
		Log::w(STR_SETTINGS_OPEN_ERROR);
		return;
	}

	for (auto &sett : this->settings)
	{
		auto search = root.find(sett.getKey());
		if (search == root.end())
		{
			Log::w(STR_SETTINGS_KEY_MISSING, PATH_SETTINGS, sett.getKey().c_str());
			continue;
		}

		try
		{
			sett.loadFromJson(search.value());
		}
		catch (const json::type_error &ex)
		{
			Log::w(STR_INVALID_TYPE_EX, PATH_SETTINGS, sett.getKey().c_str(), ex.what());
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
