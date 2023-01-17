#pragma once

#include <string>
#include "setting.hpp"

template<typename T>
class GenericSetting : public Setting
{
	protected:
		const T defaultVal;
		T &val;
		GenericSetting(const std::string &key, T &val, T defaultVal) :
			Setting(key),
			defaultVal(defaultVal),
			val(val)
		{
			this->resetToDefault();
		}

	public:
		void resetToDefault() override
		{
			this->val = this->defaultVal;
		}
};
