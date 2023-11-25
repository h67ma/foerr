// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "../hud/log.hpp"
#include "generic_setting.hpp"

template<typename T>
class EnumSetting : public GenericSetting<T>
{
	private:
		uint maxEnumValue;

	public:
		EnumSetting(const std::string& key, T& val, T defaultVal, uint maxEnumValue) :
			GenericSetting<T>(key, val, defaultVal),
			maxEnumValue(maxEnumValue)
		{
		}

		std::string defaultToString() const override
		{
			return std::to_string(this->defaultVal);
		}

		nlohmann::json getJsonValue() const override
		{
			return nlohmann::json(this->val);
		}

		void loadFromJson(const nlohmann::json& node) override
		{
			int readEnum = node;
			if (readEnum >= this->maxEnumValue)
			{
				Log::w(STR_INVALID_VALUE, this->getKey().c_str(), readEnum);
				return;
			}

			this->val = static_cast<T>(readEnum);
			Log::d(STR_LOADED_SETTING_D, this->getKey().c_str(), this->val);
		}
};
