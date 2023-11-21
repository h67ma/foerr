// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "setting.hpp"

Setting::Setting(const std::string& key) : key(key) {}

std::string Setting::getKey() const
{
	return this->key;
}
