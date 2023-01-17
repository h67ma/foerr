#pragma once

#include <string>
#include "../util/serializable_color.hpp"
#include "setting.hpp"
#include "../util/json.hpp"
#include <SFML/Graphics/Color.hpp>

/**
 * Color is stored as a string in json, and as an uint in SettingsManager
 */
class ColorSetting: public Setting
{
	private:
		const SerializableColor defaultVal;
		SerializableColor &val;

	public:
		ColorSetting(const std::string &key, SerializableColor &val, SerializableColor defaultVal);
		void resetToDefault() override;
		json getJsonValue() const override;
		void loadFromJson(const json &node) override;
};
