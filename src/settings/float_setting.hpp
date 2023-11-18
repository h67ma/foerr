// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "constraint_setting.hpp"
#include "generic_setting.hpp"

class FloatSetting : public GenericSetting<float>, public ConstraintSetting<float>
{
	public:
		FloatSetting(const std::string &key, float &val, float defaultVal);
		FloatSetting(const std::string &key, float &val, float defaultVal, const std::function<bool(float)> &constraint,
					 const std::string &valueHint); // both constraint and hint must be set if one of them is set
		std::string defaultToString() const override;
		nlohmann::json getJsonValue() const override;
		void loadFromJson(const nlohmann::json &node) override;
};
