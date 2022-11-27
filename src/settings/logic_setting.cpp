#include "logic_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

LogicSetting::LogicSetting(std::string key, bool defaultVal) :
	Setting(key, defaultVal),
	defaultVal(defaultVal)
{}

void LogicSetting::resetToDefault()
{
	this->val.logic = this->defaultVal;
}

const json LogicSetting::getJsonValue()
{
	return json(this->val.logic);
}

void LogicSetting::loadFromJson(const json &node)
{
	val.logic = node;
	Log::d(STR_LOADED_SETTING_S, this->getKey().c_str(), val.logic ? "true" : "false");
}
