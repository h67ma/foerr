#pragma once

#include <string>
#include <functional>
#include <json/json.h>
#include <SFML/Graphics.hpp>
#include "../hud/hud.hpp"
#include "../consts.hpp"

enum SettingType
{
	SETTING_UINT,
	SETTING_BOOL,
	SETTING_COLOR, // note: stored in setting_value_t::numeric
	SETTING_ENUM_SCREEN_CORNER,
	SETTING_ENUM_GUI_SCALE,
};

union setting_value_t
{
	uint numeric; // note: also used to store color (non-trivial constructor my ass, we'll just store it as uint)
	bool logic;
	ScreenCorner enumScreenCorner;
	GuiScale guiScale;
};

class Setting
{
	private:
		std::string key;
		setting_value_t defaultValue;
		SettingType settingType;
		std::function<bool(uint)> constraint = nullptr;

	public:
		setting_value_t val; // solution with templates is also possible, but more messy
		void setupUint(std::string key, uint defaultValue, const std::function<bool(uint)> constraint = nullptr);
		void setupBool(std::string key, bool defaultValue);
		void setupColor(std::string key, sf::Color color);
		void setupScreenCorner(std::string key, ScreenCorner defaultValue);
		void setupGuiScale(std::string key, GuiScale guiScale);
		void resetToDefault();
		std::string getKey();
		Json::Value getJsonValue();
		void loadFromJson(Json::Value value);
};
