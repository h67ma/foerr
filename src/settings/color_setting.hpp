#pragma once

#include <string>

#include <SFML/Graphics/Color.hpp>

#include "../util/serializable_color.hpp"
#include "generic_setting.hpp"
#include "../util/json.hpp"

/**
 * Color is stored as a string in json, and as an uint in SettingsManager
 */
class ColorSetting : public GenericSetting<SerializableColor>
{
	public:
		ColorSetting(const std::string &key, SerializableColor &val, SerializableColor defaultVal);
		std::string defaultToString() const override;
		json getJsonValue() const override;
		void loadFromJson(const json &node) override;
};
