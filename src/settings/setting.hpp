#pragma once

#include <string>
#include <functional>
#include "../util/json.hpp"
#include <SFML/Graphics/Color.hpp>
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

	// wow, this sucks
	setting_value_t(uint numeric): numeric(numeric) {}
	setting_value_t(bool logic): logic(logic) {}
	setting_value_t(ScreenCorner enumScreenCorner): enumScreenCorner(enumScreenCorner) {}
	setting_value_t(GuiScale guiScale): guiScale(guiScale) {}
};

class Setting
{
	private:
		const std::string key; // for serialization
		const setting_value_t defaultValue;
		const SettingType settingType;
		const std::function<bool(uint)> constraint;

	public:
		setting_value_t val; // solution with templates is also possible, but more messy
		explicit Setting(std::string key, uint defaultValue, const std::function<bool(uint)> constraint = nullptr);
		explicit Setting(std::string key, bool defaultValue);
		explicit Setting(std::string key, sf::Color color);
		explicit Setting(std::string key, ScreenCorner defaultValue);
		explicit Setting(std::string key, GuiScale guiScale);
		void resetToDefault();
		const std::string getKey();
		const json getJsonValue();
		void loadFromJson(const json &node);
};
