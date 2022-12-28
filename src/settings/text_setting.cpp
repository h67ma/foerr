#include "text_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

TextSetting::TextSetting(std::string key, std::string defaultVal) :
	Setting(key, defaultVal),
	defaultVal(defaultVal)
{}

void TextSetting::resetToDefault()
{
	this->textVal = this->defaultVal;
}

const json TextSetting::getJsonValue()
{
	return json(this->textVal);
}

void TextSetting::loadFromJson(const json &node)
{
	this->textVal = node;
	Log::d(STR_LOADED_SETTING_SQ, this->getKey().c_str(), this->textVal.c_str());
}
