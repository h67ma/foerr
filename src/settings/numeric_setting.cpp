#include "numeric_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

NumericSetting::NumericSetting(const std::string &key, uint defaultVal) :
	Setting(key, defaultVal),
	defaultVal(defaultVal),
	constraint(nullptr),
	valueHint("")
{}

NumericSetting::NumericSetting(const std::string &key, uint defaultVal, const std::function<bool(uint)> constraint,
							   const std::string &valueHint) :
	Setting(key, defaultVal),
	defaultVal(defaultVal),
	constraint(constraint),
	valueHint(valueHint)
{}

void NumericSetting::resetToDefault()
{
	this->val.numeric = this->defaultVal;
}

const json NumericSetting::getJsonValue() const
{
	return json(this->val.numeric);
}

void NumericSetting::loadFromJson(const json &node)
{
	uint readUint = node;
	if (this->constraint != nullptr && !this->constraint(readUint))
	{
		Log::w(STR_INVALID_VALUE_HINT, this->getKey().c_str(), readUint, this->valueHint.c_str());
	}
	else
	{
		val.numeric = readUint;
		Log::d(STR_LOADED_SETTING_U, this->getKey().c_str(), val.numeric);
	}
}
