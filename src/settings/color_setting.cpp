#include "color_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/serializable_color.hpp"

ColorSetting::ColorSetting(const std::string &key, sf::Color defaultVal) :
	Setting(key, defaultVal.toInteger()),
	defaultVal(defaultVal.toInteger())
{}

void ColorSetting::resetToDefault()
{
	this->val.numeric = this->defaultVal;
}

const json ColorSetting::getJsonValue()
{
	SerializableColor color = SerializableColor(this->val.numeric);
	return json(color.toString());
}

void ColorSetting::loadFromJson(const json &node)
{
	SerializableColor readColor;
	std::string readString = node;
	if (!readColor.loadFromColorString(readString))
	{
		Log::w(STR_INVALID_VALUE_COLOR, this->getKey().c_str(), readString.c_str());
		return;
	}

	val.numeric = readColor.toInteger();
	Log::d(STR_LOADED_SETTING_S, this->getKey().c_str(), readString.c_str());
}
