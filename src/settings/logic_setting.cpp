#include "logic_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

LogicSetting::LogicSetting(const std::string &key, bool &val, bool defaultVal) :
	Setting(key),
	val(val),
	defaultVal(defaultVal)
{
	this->resetToDefault();
}

void LogicSetting::resetToDefault()
{
	this->val = this->defaultVal;
}

json LogicSetting::getJsonValue() const
{
	return json(this->val);
}

void LogicSetting::loadFromJson(const json &node)
{
	this->val = node;
	Log::d(STR_LOADED_SETTING_S, this->getKey().c_str(), this->val ? "true" : "false");
}
