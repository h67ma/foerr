#pragma once

#include <string>
#include "setting.hpp"
#include "../hud/hud.hpp"
#include "../util/json.hpp"

class ScreenCornerSetting: public Setting
{
	private:
		const ScreenCorner defaultVal;

	public:
		ScreenCornerSetting(const std::string &key, ScreenCorner defaultVal);
		void resetToDefault() override;
		const json getJsonValue() override;
		void loadFromJson(const json &node) override;
};
