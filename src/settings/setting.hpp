#pragma once

#include <string>
#include "../util/json.hpp"
#include "../hud/hud.hpp"
#include "../consts.hpp"

union setting_value_t
{
	uint numeric; // note: also used to store color (non-trivial constructor my ass, we'll just store it as uint)
	bool logic;
	ScreenCorner enumScreenCorner;
	GuiScale guiScale;

	// wow, this sucks
	setting_value_t() = default;
	setting_value_t(uint numeric): numeric(numeric) {}
	setting_value_t(bool logic): logic(logic) {}
	setting_value_t(ScreenCorner enumScreenCorner): enumScreenCorner(enumScreenCorner) {}
	setting_value_t(GuiScale guiScale): guiScale(guiScale) {}
};

class Setting
{
	private:
		const std::string key; // for serialization

	protected:
		explicit Setting(const std::string &key, uint value);
		explicit Setting(const std::string &key, bool value);
		explicit Setting(const std::string &key, const std::string &value);
		explicit Setting(const std::string &key, GuiScale guiScale);
		explicit Setting(const std::string &key, ScreenCorner defaultValue);

	public:
		setting_value_t val; // solution with templates is also possible, but more messy
		std::string textVal; // TODO find a way to make this all ok

		const std::string getKey();
		virtual void resetToDefault() = 0;
		const virtual json getJsonValue() const = 0;
		void virtual loadFromJson(const json &node) = 0;
};
