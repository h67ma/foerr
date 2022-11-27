#pragma once

#include <string>
#include "setting.hpp"
#include "../util/json.hpp"

class TextSetting: public Setting
{
	private:
		const std::string defaultVal;

	public:
		TextSetting(std::string key, std::string defaultVal);
		void resetToDefault() override;
		const json getJsonValue() override;
		void loadFromJson(const json &node) override;
};
