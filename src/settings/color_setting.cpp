#include "color_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

ColorSetting::ColorSetting(const std::string &key, SerializableColor &val, SerializableColor defaultVal) :
	Setting(key),
	val(val),
	defaultVal(defaultVal)
{
	this->resetToDefault();
}

void ColorSetting::resetToDefault()
{
	this->val = this->defaultVal;
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
