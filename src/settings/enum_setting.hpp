#pragma once

#include <string>
#include "../util/json.hpp"
#include "../hud/log.hpp"
#include "setting.hpp"

template<typename T>
class EnumSetting : public Setting
{
	private:
		const T defaultVal;
		T &val;
		uint maxEnumValue;

	public:
		EnumSetting(const std::string &key, T &val, T defaultVal, uint maxEnumValue) :
			Setting(key),
			val(val),
			defaultVal(defaultVal),
			maxEnumValue(maxEnumValue)
		{
			this->resetToDefault();
		}

		void resetToDefault() override
		{
			this->val = this->defaultVal;
		}

		json getJsonValue() const override
		{
			return json(this->val);
		}

		void loadFromJson(const json &node) override
		{
			int readEnum = node;
			if (readEnum >= this->maxEnumValue)
			{
				Log::w(STR_INVALID_VALUE, this->getKey().c_str(), readEnum);
				return;
			}

			this->val = static_cast<T>(readEnum);
			Log::d(STR_LOADED_SETTING_D, this->getKey().c_str(), val);
		}
};
