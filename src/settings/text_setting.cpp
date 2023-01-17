#include "text_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

TextSetting::TextSetting(const std::string &key, std::string &val, const std::string &defaultVal) :
	Setting(key),
	val(val),
	defaultVal(defaultVal)
{
	this->resetToDefault();
}

void TextSetting::resetToDefault()
{
	this->val = this->defaultVal;
}

json TextSetting::getJsonValue() const
{
	return json(this->val);
}

void TextSetting::loadFromJson(const json &node)
{
	this->val = node;
	Log::d(STR_LOADED_SETTING_SQ, this->getKey().c_str(), this->val.c_str());
}
