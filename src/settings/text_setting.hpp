#pragma once

#include <string>

#include "generic_setting.hpp"
#include "../util/json.hpp"

class TextSetting : public GenericSetting<std::string>
{
	public:
		TextSetting(const std::string &key, std::string &val, const std::string &defaultVal);
		std::string defaultToString() const override;
		nlohmann::json getJsonValue() const override;
		void loadFromJson(const nlohmann::json &node) override;
};
