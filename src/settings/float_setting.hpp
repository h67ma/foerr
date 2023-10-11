#pragma once

#include <string>

#include "constraint_setting.hpp"
#include "generic_setting.hpp"
#include "../util/json.hpp"

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
