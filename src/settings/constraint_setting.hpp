// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <functional>
#include <string>

template<typename T>
class ConstraintSetting
{
	protected:
		ConstraintSetting(const std::function<bool(T)>& constraint, const std::string& valueHint) :
			constraint(constraint), valueHint(valueHint) {}
		const std::function<bool(T)> constraint;
		const std::string valueHint; // describes valid values of the setting
};
