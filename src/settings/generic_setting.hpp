// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include "setting.hpp"

/**
 * Note: Setting cannot be merged with this template, even though it's not actually used by anything else.
 * A non-template superclass is needed in order to create a generic collection of Settings.
 */
template<typename T>
class GenericSetting : public Setting
{
	protected:
		const T defaultVal;
		T& val;
		GenericSetting(const std::string& key, T& val, T defaultVal) :
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
