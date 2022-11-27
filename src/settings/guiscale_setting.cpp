#include "guiscale_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

GuiScaleSetting::GuiScaleSetting(std::string key, GuiScale defaultVal) :
	Setting(key, defaultVal),
	defaultVal(defaultVal)
{}

void GuiScaleSetting::resetToDefault()
{
	this->val.guiScale = this->defaultVal;
}

const json GuiScaleSetting::getJsonValue()
{
	return json(this->val.numeric);
}

void GuiScaleSetting::loadFromJson(const json &node)
{
	int readEnum = node;
	if (readEnum >= _GUI_SCALE_CNT)
	{
		Log::w(STR_INVALID_VALUE, this->getKey().c_str(), readEnum);
		return;
	}

	val.guiScale = static_cast<GuiScale>(readEnum);
	Log::d(STR_LOADED_SETTING_D, this->getKey().c_str(), val.guiScale);
}
