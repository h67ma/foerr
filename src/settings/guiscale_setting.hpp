#pragma once

#include <string>
#include "setting.hpp"
#include "../hud/hud.hpp"
#include "../util/json.hpp"

class GuiScaleSetting: public Setting
{
	private:
		const GuiScale defaultVal;

	public:
		GuiScaleSetting(const std::string &key, GuiScale defaultVal);
		void resetToDefault() override;
		const json getJsonValue() override;
		void loadFromJson(const json &node) override;
};
