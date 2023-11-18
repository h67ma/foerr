// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

class ConfigurableGuiComponent
{
	public:
		virtual void handleSettingsChange() = 0;
};
