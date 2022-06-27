#pragma once

#include <string>
#include <fstream>
#include "../hud/hud.hpp"

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
	public:
		setting_value_t val; // solution with templates is also possible, but more messy
		void setup(std::string key, uint defaultValue);
		void setup(std::string key, bool defaultValue);
		void setup(std::string key, ScreenCorner defaultValue);
		void resetToDefault();
		void writeToFile(std::ofstream& file);
		bool tryLoadFromLine(std::string line);
};
