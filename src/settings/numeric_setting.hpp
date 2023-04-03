#pragma once

#include <functional>
#include <string>

#include "constraint_setting.hpp"
#include "generic_setting.hpp"
#include "../util/json.hpp"
#include "../consts.hpp"

class NumericSetting : public GenericSetting<uint>, public ConstraintSetting<uint>
{
	public:
		NumericSetting(const std::string &key, uint &val, uint defaultVal);
		NumericSetting(const std::string &key, uint &val, uint defaultVal, const std::function<bool(uint)> &constraint,
					   const std::string &valueHint); // both constraint and hint must be set if one of them is set
		std::string defaultToString() const override;
		json getJsonValue() const override;
		void loadFromJson(const json &node) override;
};
