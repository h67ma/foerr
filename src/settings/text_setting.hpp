#pragma once

#include <string>
#include "setting.hpp"
#include "../util/json.hpp"

class TextSetting: public Setting
{
	private:
		const std::string defaultVal;
		std::string &val;

	public:
		TextSetting(const std::string &key, std::string &val, const std::string &defaultVal);
		void resetToDefault() override;
		json getJsonValue() const override;
		void loadFromJson(const json &node) override;
};
