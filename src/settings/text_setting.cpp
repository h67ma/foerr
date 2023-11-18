// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "text_setting.hpp"

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

TextSetting::TextSetting(const std::string &key, std::string &val, const std::string &defaultVal) :
	GenericSetting<std::string>(key, val, defaultVal) {}

std::string TextSetting::defaultToString() const
{
	return this->defaultVal;
}

nlohmann::json TextSetting::getJsonValue() const
{
	return nlohmann::json(this->val);
}

void TextSetting::loadFromJson(const nlohmann::json &node)
{
	this->val = node;
	Log::d(STR_LOADED_SETTING_SQ, this->getKey().c_str(), this->val.c_str());
}
