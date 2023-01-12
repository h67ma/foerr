#pragma once

#include <functional>
#include <string>
#include "setting.hpp"
#include "../consts.hpp"
#include "../util/json.hpp"

class NumericSetting: public Setting
{
	private:
		const uint defaultVal;
		const std::function<bool(uint)> constraint;
		const std::string valueHint = ""; // describes valid values of the setting

	public:
		NumericSetting(const std::string &key, uint defaultVal);
		NumericSetting(const std::string &key, uint defaultVal, const std::function<bool(uint)> constraint,
					   const std::string &valueHint); // both constraint and hint must be set if one of them is set
		void resetToDefault() override;
		const json getJsonValue() override;
		void loadFromJson(const json &node) override;
};
