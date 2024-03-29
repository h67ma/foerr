// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "generic_setting.hpp"

class TextSetting : public GenericSetting<std::string>
{
	public:
		TextSetting(const std::string& key, std::string& val, const std::string& defaultVal);
		std::string defaultToString() const override;
		nlohmann::json getJsonValue() const override;
		void loadFromJson(const nlohmann::json& node) override;
};
