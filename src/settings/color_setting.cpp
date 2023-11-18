// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "color_setting.hpp"

#include "../util/i18n.hpp"
#include "../hud/log.hpp"

ColorSetting::ColorSetting(const std::string &key, SerializableColor &val, SerializableColor defaultVal) :
	GenericSetting<SerializableColor>(key, val, defaultVal) {}

std::string ColorSetting::defaultToString() const
{
	return this->defaultVal.toString();
}

nlohmann::json ColorSetting::getJsonValue() const
{
	return nlohmann::json(this->val.toString());
}

void ColorSetting::loadFromJson(const nlohmann::json &node)
{
	std::string readString = node;
	if (!this->val.loadFromColorString(readString))
	{
		Log::w(STR_INVALID_VALUE_COLOR, this->getKey().c_str(), readString.c_str());
		return;
	}

	Log::d(STR_LOADED_SETTING_S, this->getKey().c_str(), readString.c_str());
}
