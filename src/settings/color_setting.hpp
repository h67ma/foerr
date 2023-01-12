#pragma once

#include <string>
#include "setting.hpp"
#include "../util/json.hpp"
#include <SFML/Graphics/Color.hpp>

/**
 * Color is stored as a string in json, and as an uint in SettingsManager
 */
class ColorSetting: public Setting
{
	private:
		const uint defaultVal;

	public:
		ColorSetting(const std::string &key, sf::Color defaultVal);
		void resetToDefault() override;
		const json getJsonValue() override;
		void loadFromJson(const json &node) override;
};
