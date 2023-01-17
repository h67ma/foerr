#include "logic_setting.hpp"

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

LogicSetting::LogicSetting(const std::string &key, bool &val, bool defaultVal) :
	GenericSetting<bool>(key, val, defaultVal) {}

std::string LogicSetting::defaultToString() const
{
	return this->defaultVal ? "true" : "false";
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
