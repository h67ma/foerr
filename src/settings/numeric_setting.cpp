// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "numeric_setting.hpp"

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

NumericSetting::NumericSetting(const std::string& key, uint& val, uint defaultVal) :
	GenericSetting(key, val, defaultVal),
	ConstraintSetting(nullptr, "") {}

NumericSetting::NumericSetting(const std::string& key, uint& val, uint defaultVal,
							   const std::function<bool(uint)>& constraint, const std::string& valueHint) :
	GenericSetting(key, val, defaultVal),
	ConstraintSetting(constraint, valueHint) {}

std::string NumericSetting::defaultToString() const
{
	return std::to_string(this->defaultVal);
}

nlohmann::json NumericSetting::getJsonValue() const
{
	// return default value if constraint is not met on current value
	if (this->constraint != nullptr && !this->constraint(this->val))
		return nlohmann::json(this->defaultVal);

	return nlohmann::json(this->val);
}

void NumericSetting::loadFromJson(const nlohmann::json& node)
{
	uint readUint = node;
	if (this->constraint != nullptr && !this->constraint(readUint))
	{
		Log::w(STR_INVALID_VALUE_HINT_U, this->getKey().c_str(), readUint, this->valueHint.c_str());
	}
	else
	{
		this->val = readUint;
		Log::d(STR_LOADED_SETTING_U, this->getKey().c_str(), this->val);
	}
}
