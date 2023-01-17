#include "screencorner_setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

ScreenCornerSetting::ScreenCornerSetting(const std::string &key, ScreenCorner &val, ScreenCorner defaultVal) :
	Setting(key),
	val(val),
	defaultVal(defaultVal)
{
	this->resetToDefault();
}

void ScreenCornerSetting::resetToDefault()
{
	this->val = this->defaultVal;
}

json ScreenCornerSetting::getJsonValue() const
{
	return json(this->val);
}

void ScreenCornerSetting::loadFromJson(const json &node)
{
	int readEnum = node;
	if (readEnum >= _CORNER_CNT)
	{
		Log::w(STR_INVALID_VALUE, this->getKey().c_str(), readEnum);
		return;
	}

	this->val = static_cast<ScreenCorner>(readEnum);
	Log::d(STR_LOADED_SETTING_D, this->getKey().c_str(), val);
}
