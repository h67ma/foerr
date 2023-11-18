// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <functional>
#include <string>

#include <nlohmann/json.hpp>

#include "constraint_setting.hpp"
#include "generic_setting.hpp"
#include "../consts.hpp"

class NumericSetting : public GenericSetting<uint>, public ConstraintSetting<uint>
{
	public:
		NumericSetting(const std::string &key, uint &val, uint defaultVal);
		NumericSetting(const std::string &key, uint &val, uint defaultVal, const std::function<bool(uint)> &constraint,
					   const std::string &valueHint); // both constraint and hint must be set if one of them is set
		std::string defaultToString() const override;
		nlohmann::json getJsonValue() const override;
		void loadFromJson(const nlohmann::json &node) override;
};
