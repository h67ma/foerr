#include "color_setting.hpp"

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

ColorSetting::ColorSetting(const std::string &key, SerializableColor &val, SerializableColor defaultVal) :
	GenericSetting<SerializableColor>(key, val, defaultVal) {}

std::string ColorSetting::defaultToString() const
{
	return this->defaultVal.toString();
}

json ColorSetting::getJsonValue() const
{
	return json(this->val.toString());
}

void ColorSetting::loadFromJson(const json &node)
{
	std::string readString = node;
	if (!this->val.loadFromColorString(readString))
	{
		Log::w(STR_INVALID_VALUE_COLOR, this->getKey().c_str(), readString.c_str());
		return;
	}

	Log::d(STR_LOADED_SETTING_S, this->getKey().c_str(), readString.c_str());
}
