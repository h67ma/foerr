#include "numeric_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

NumericSetting::NumericSetting(const std::string &key, uint &val, uint defaultVal) :
	Setting(key),
	val(val),
	defaultVal(defaultVal),
	constraint(nullptr),
	valueHint("")
{
	this->resetToDefault();
}

NumericSetting::NumericSetting(const std::string &key, uint &val, uint defaultVal,
							   const std::function<bool(uint)> constraint, const std::string &valueHint) :
	Setting(key),
	val(val),
	defaultVal(defaultVal),
	constraint(constraint),
	valueHint(valueHint)
{
	this->resetToDefault();
}

void NumericSetting::resetToDefault()
{
	this->val = this->defaultVal;
}

json NumericSetting::getJsonValue() const
{
	// return default value if constraint is not met on current value
	if (this->constraint != nullptr && !this->constraint(this->val))
		return json(this->defaultVal);

	return json(this->val);
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
		this->val = readUint;
		Log::d(STR_LOADED_SETTING_U, this->getKey().c_str(), this->val);
	}
}
