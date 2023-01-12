#pragma once

#include <string>
#include "setting.hpp"
#include "../util/json.hpp"

class LogicSetting: public Setting
{
	private:
		const bool defaultVal;

	public:
		LogicSetting(const std::string &key, bool defaultVal);
		void resetToDefault() override;
		const json getJsonValue() override;
		void loadFromJson(const json &node) override;
};
