#pragma once

#include <functional>
#include <string>

#include "generic_setting.hpp"
#include "../util/json.hpp"
#include "../consts.hpp"

/**
 * Note: other settings also could support constraints, but unfortunately it's pretty hard to extract that behaviour
 * as a superclass/interface.
 */
class NumericSetting : public GenericSetting<uint>
{
	private:
		const std::function<bool(uint)> constraint;
		const std::string valueHint; // describes valid values of the setting

	public:
		NumericSetting(const std::string &key, uint &val, uint defaultVal);
		NumericSetting(const std::string &key, uint &val, uint defaultVal, const std::function<bool(uint)> constraint,
					   const std::string &valueHint); // both constraint and hint must be set if one of them is set
		std::string defaultToString() const override;
		json getJsonValue() const override;
		void loadFromJson(const json &node) override;
};
