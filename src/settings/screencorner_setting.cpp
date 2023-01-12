#include "screencorner_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

ScreenCornerSetting::ScreenCornerSetting(const std::string &key, ScreenCorner defaultVal) :
	Setting(key, defaultVal),
	defaultVal(defaultVal)
{}

void ScreenCornerSetting::resetToDefault()
{
	this->val.enumScreenCorner = this->defaultVal;
}

const json ScreenCornerSetting::getJsonValue()
{
	return json(this->val.numeric);
}

void ScreenCornerSetting::loadFromJson(const json &node)
{
	int readEnum = node;
	if (readEnum >= _CORNER_CNT)
	{
		Log::w(STR_INVALID_VALUE, this->getKey().c_str(), readEnum);
		return;
	}

	val.enumScreenCorner = static_cast<ScreenCorner>(readEnum);
	Log::d(STR_LOADED_SETTING_D, this->getKey().c_str(), val.enumScreenCorner);
}
