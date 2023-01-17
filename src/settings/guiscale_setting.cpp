#include "guiscale_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

GuiScaleSetting::GuiScaleSetting(const std::string &key, GuiScale &val, GuiScale defaultVal) :
	Setting(key),
	val(val),
	defaultVal(defaultVal)
{
	this->resetToDefault();
}

void GuiScaleSetting::resetToDefault()
{
	this->val = this->defaultVal;
}

json GuiScaleSetting::getJsonValue() const
{
	return json(this->val);
}

void GuiScaleSetting::loadFromJson(const json &node)
{
	int readEnum = node;
	if (readEnum >= _GUI_SCALE_CNT)
	{
		Log::w(STR_INVALID_VALUE, this->getKey().c_str(), readEnum);
		return;
	}

	this->val = static_cast<GuiScale>(readEnum);
	Log::d(STR_LOADED_SETTING_D, this->getKey().c_str(), val);
}
