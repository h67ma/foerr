#pragma once

#include <string>

#include "generic_setting.hpp"
#include "../util/json.hpp"

class LogicSetting : public GenericSetting<bool>
{
	public:
		LogicSetting(const std::string &key, bool &val, bool defaultVal);
		std::string defaultToString() const override;
		nlohmann::json getJsonValue() const override;
		void loadFromJson(const nlohmann::json &node) override;
};
