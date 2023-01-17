#pragma once

#include <string>
#include "setting.hpp"
#include "../hud/hud.hpp"
#include "../util/json.hpp"

class ScreenCornerSetting: public Setting
{
	private:
		const ScreenCorner defaultVal;
		ScreenCorner &val;

	public:
		ScreenCornerSetting(const std::string &key, ScreenCorner &val, ScreenCorner defaultVal);
		void resetToDefault() override;
		json getJsonValue() const override;
		void loadFromJson(const json &node) override;
};
