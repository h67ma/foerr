// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "logic_setting.hpp"

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

LogicSetting::LogicSetting(const std::string &key, bool &val, bool defaultVal) :
	GenericSetting<bool>(key, val, defaultVal) {}

std::string LogicSetting::defaultToString() const
{
	return this->defaultVal ? "true" : "false";
}

nlohmann::json LogicSetting::getJsonValue() const
{
	return nlohmann::json(this->val);
}

void LogicSetting::loadFromJson(const nlohmann::json &node)
{
	this->val = node;
	Log::d(STR_LOADED_SETTING_S, this->getKey().c_str(), this->val ? "true" : "false");
}
