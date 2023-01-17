#pragma once

#include <string>
#include "setting.hpp"
#include "../util/json.hpp"

class LogicSetting: public Setting
{
	private:
		const bool defaultVal;
		bool &val;

	public:
		LogicSetting(const std::string &key, bool &val, bool defaultVal);
		std::string defaultToString() const override;
		void resetToDefault() override;
		json getJsonValue() const override;
		void loadFromJson(const json &node) override;
};
