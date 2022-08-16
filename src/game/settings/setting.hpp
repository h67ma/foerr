#pragma once

#include <string>
#include <json/json.h>
#include "../hud/hud.hpp"
#include "../consts.hpp"

enum SettingType
{
	SETTING_UINT,
	SETTING_BOOL,
	SETTING_ENUM_SCREEN_CORNER,
	SETTING_ENUM_HUD_SCALE,
};

union setting_value_t
{
	uint numeric;
	bool logic;
	ScreenCorner enumScreenCorner;
	HudScale hudScale;
};

class Setting
{
	private:
		std::string key;
		setting_value_t defaultValue;
		SettingType settingType;
	public:
		setting_value_t val; // solution with templates is also possible, but more messy
		void setup(std::string key, uint defaultValue);
		void setup(std::string key, bool defaultValue);
		void setup(std::string key, ScreenCorner defaultValue);
		void setup(std::string key, HudScale hudScale);
		void resetToDefault();
		std::string getKey();
		Json::Value getJsonValue();
		void loadFromJson(Json::Value value);
};
