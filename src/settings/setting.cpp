#include "setting.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/color.hpp"

void Setting::setupUint(std::string key, uint defaultValue, const std::function<bool(uint)> constraint)
{
	this->key = key;
	this->defaultValue.numeric = defaultValue;
	this->val.numeric = defaultValue;
	this->constraint = constraint;
	this->settingType = SETTING_UINT;
}

void Setting::setupBool(std::string key, bool defaultValue)
{
	this->key = key;
	this->defaultValue.logic = defaultValue;
	this->val.logic = defaultValue;
	this->settingType = SETTING_BOOL;
}

void Setting::setupColor(std::string key, sf::Color color)
{
	this->key = key;
	this->defaultValue.numeric = color.toInteger();
	this->val.numeric = color.toInteger();
	this->settingType = SETTING_COLOR;
}

void Setting::setupScreenCorner(std::string key, ScreenCorner defaultValue)
{
	this->key = key;
	this->defaultValue.enumScreenCorner = defaultValue;
	this->val.enumScreenCorner = defaultValue;
	this->settingType = SETTING_ENUM_SCREEN_CORNER;
}

void Setting::setupGuiScale(std::string key, GuiScale defaultValue)
{
	this->key = key;
	this->defaultValue.guiScale = defaultValue;
	this->val.guiScale = defaultValue;
	this->settingType = SETTING_ENUM_GUI_SCALE;
}

// TODO do we actually need this?
void Setting::resetToDefault()
{
	Log::d(STR_RESETTING_TO_DEFAULT, this->key.c_str());
	switch (this->settingType)
	{
		case SETTING_BOOL:
			this->val.logic = this->defaultValue.logic;
			break;
		case SETTING_ENUM_SCREEN_CORNER:
			this->val.enumScreenCorner = this->defaultValue.enumScreenCorner;
			break;
		case SETTING_ENUM_GUI_SCALE:
			this->val.guiScale = this->defaultValue.guiScale;
			break;
		case SETTING_UINT:
		case SETTING_COLOR:
		default:
			this->val.numeric = this->defaultValue.numeric;
	}
}

std::string Setting::getKey()
{
	return this->key;
}

/**
 * @returns setting's value, as Json node
 */
json Setting::getJsonValue()
{
	Color color;

	switch (this->settingType)
	{
		case SETTING_BOOL:
			return json(val.logic);
			break;
		case SETTING_COLOR:
			color = Color(val.numeric);
			return json(color.toString());
		case SETTING_ENUM_SCREEN_CORNER: // enums are ints anyway
		case SETTING_ENUM_GUI_SCALE: // ditto
		case SETTING_UINT:
		default:
			return json(val.numeric);
	}
}

/**
 * Loads value from Json node. Assumes the node's type is the same as setting's type.
 *
 * @param value Json node with an elementary value
 */
void Setting::loadFromJson(const json &node)
{
	if (this->settingType == SETTING_BOOL)
	{
		val.logic = node;
		Log::d(STR_LOADED_SETTING_D, key.c_str(), val.logic);
	}
	else if (this->settingType == SETTING_COLOR)
	{
		Color readColor;
		std::string readString = node;
		if (!readColor.loadFromColorString(readString))
		{
			Log::w(STR_INVALID_COLOR_VALUE, readString.c_str(), key.c_str());
			return;
		}

		val.numeric = readColor.toInteger();
		Log::d(STR_LOADED_SETTING_S, key.c_str(), readString.c_str());
	}
	else if (this->settingType == SETTING_ENUM_SCREEN_CORNER )
	{
		int readEnum = node;
		if (readEnum >= _CORNER_CNT)
		{
			Log::w(STR_INVALID_VALUE, key.c_str(), readEnum);
			return;
		}

		val.enumScreenCorner = static_cast<ScreenCorner>(readEnum);
		Log::d(STR_LOADED_SETTING_D, key.c_str(), val.enumScreenCorner);
	}
	else if (this->settingType == SETTING_ENUM_GUI_SCALE)
	{
		int readEnum = node;
		if (readEnum >= _GUI_SCALE_CNT)
		{
			Log::w(STR_INVALID_VALUE, key.c_str(), readEnum);
			return;
		}

		val.guiScale = static_cast<GuiScale>(readEnum);
		Log::d(STR_LOADED_SETTING_D, key.c_str(), val.guiScale);
	}
	else // SETTING_UINT
	{
		uint readUint = node;
		if (this->constraint != nullptr && !this->constraint(readUint))
		{
			Log::w(STR_INVALID_VALUE, key.c_str(), readUint);
		}
		else
		{
			val.numeric = readUint;
			Log::d(STR_LOADED_SETTING_U, key.c_str(), val.numeric);
		}
	}
}
