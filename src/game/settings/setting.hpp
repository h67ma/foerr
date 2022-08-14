#pragma once

#include <string>
#include <json/json.h>
#include "../hud/hud.hpp"
#include "../consts.hpp"

enum SettingType
{
	SETTING_UINT,
	SETTING_BOOL,
	SETTING_ENUM_SCREEN_CORNER
};

union setting_value_t
{
	uint numeric;
	bool logic;
	ScreenCorner enumScreenCorner;
};

class Setting
{
	private:
		std::string key;
		setting_value_t defaultValue;
		SettingType settingType;
		bool debug; // debug settings can be loaded from settings file, but are never written
	public:
		setting_value_t val; // solution with templates is also possible, but more messy
		void setup(std::string key, uint defaultValue, bool debug=false);
		void setup(std::string key, bool defaultValue, bool debug=false);
		void setup(std::string key, ScreenCorner defaultValue, bool debug=false);
		void resetToDefault();
		std::string getKey();
		bool isDebug();
		Json::Value getJsonValue();
		void loadFromJson(Json::Value value);
};
