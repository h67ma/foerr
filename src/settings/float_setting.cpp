// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include "float_setting.hpp"

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

FloatSetting::FloatSetting(const std::string& key, float& val, float defaultVal) :
	GenericSetting(key, val, defaultVal),
	ConstraintSetting(nullptr, "") {}

FloatSetting::FloatSetting(const std::string& key, float& val, float defaultVal,
						   const std::function<bool(float)>& constraint, const std::string& valueHint) :
	GenericSetting(key, val, defaultVal),
	ConstraintSetting(constraint, valueHint) {}

std::string FloatSetting::defaultToString() const
{
	return std::to_string(this->defaultVal);
}

nlohmann::json FloatSetting::getJsonValue() const
{
	// return default value if constraint is not met on current value
	if (this->constraint != nullptr && !this->constraint(this->val))
		return nlohmann::json(this->defaultVal);

	return nlohmann::json(this->val);
}

void FloatSetting::loadFromJson(const nlohmann::json& node)
{
	float readFloat = node;
	if (this->constraint != nullptr && !this->constraint(readFloat))
	{
		Log::w(STR_INVALID_VALUE_HINT_F, this->getKey().c_str(), readFloat, this->valueHint.c_str());
	}
	else
	{
		this->val = readFloat;
		Log::d(STR_LOADED_SETTING_F, this->getKey().c_str(), this->val);
	}
}
