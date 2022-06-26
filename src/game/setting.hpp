#pragma once

#include <string>
#include <fstream>
#include "gui.hpp"

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
		Setting(std::string key, uint defaultValue);
		Setting(std::string key, bool defaultValue);
		Setting(std::string key, ScreenCorner defaultValue);
		void resetToDefault();
		void writeToFile(std::ofstream& file);
		bool tryLoadFromLine(std::string line);
};
